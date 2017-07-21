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
#define LOG_TAG "awb_mgr_3"

#ifndef ENABLE_MY_LOG
#define ENABLE_MY_LOG       (1)
#endif

#include <cutils/properties.h>
#include <stdlib.h>
#include <aaa_types.h>
#include <aaa_error_code.h>
#include <aaa_log.h>
//#include <aaa_hal.h>
#include <camera_custom_nvram.h>
#include <awb_feature.h>
#include <flash_awb_param.h>
#include <flash_awb_tuning_custom.h>
#include <awb_param.h>
#include <nvram_drv_mgr.h>
#include <awb_tuning_custom.h>
#include <isp_mgr.h>
#include <isp_tuning_mgr.h>
//#include <kd_camera_feature.h>
#include <isp_tuning.h>
#include <tuning_mgr.h>
#include <camera_custom_cam_cal.h>
#include <cam_cal_drv.h>
#include <client/mtk_metadata_tag.h>
#include <flash_feature.h>
#include <awb_state.h>
#include <IHalSensor.h>
#include <iopipe/CamIO/INormalPipe.h>
#include "awb_mgr.h"
#include <nvbuf_util.h>
#if 1//iopipe2.0
#define IOPIPE_SET_MODUL_REG(handle,RegName,Value)
#define IOPIPE_SET_MODUL_REGS(handle, StartRegName, size, ValueArray)
namespace NSImageio {
    namespace NSIspio {
        enum EModule
        {
            //raw
            EModule_OB          = 00,
            EModule_BNR         = 05,
            EModule_LSC         = 10,
            EModule_RPG         = 15,
            EModule_AE          = 20,
            EModule_AWB         = 25,
            EModule_SGG1        = 30,
            EModule_FLK         = 35,
            EModule_AF          = 40,
            EModule_SGG2        = 45,
            EModule_SGG3        = 46,
            EModule_EIS         = 50,
            EModule_LCS         = 55,
            EModule_BPCI        = 60,
            EModule_LSCI        = 65,
            EModule_AAO         = 70,
            EModule_ESFKO       = 75,
            EModule_AFO         = 80,
            EModule_EISO        = 85,
            EModule_LCSO        = 90,
            EModule_iHDR        = 95,
            EModule_CAMSV_IMGO  = 100,
            //raw_d
            EModule_OB_D        = 1000,
            EModule_BNR_D         = 1005,
            EModule_LSC_D       = 1010,
            EModule_RPG_D       = 1015,
            EModule_BPCI_D      = 1020,
            EModule_LSCI_D      = 1025,
            EModule_AE_D        = 1030,
            EModule_AWB_D       = 1035,
            EModule_SGG1_D      = 1040,
            EModule_AF_D        = 1045,
            EModule_LCS_D       = 1050,
            EModule_AAO_D       = 1055,
            EModule_AFO_D       = 1060,
            EModule_LCSO_D      = 1065,
            EModule_iHDR_D      = 1070
        };
        enum EPIPECmd {

            //  IPECmd_SET_SENSOR_DEV             = 0x1001,
            //  EPIPECmd_SET_SENSOR_GAIN            = 0x1002,
            //  EPIPECmd_SET_SENSOR_EXP             = 0x1003,
            //  EPIPECmd_SET_CAM_MODE               = 0x1004,
            EPIPECmd_SET_SCENE_MODE             = 0x1005,
            EPIPECmd_SET_ISO                    = 0x1006,
            EPIPECmd_SET_FLUORESCENT_CCT        = 0x1007,
            EPIPECmd_SET_SCENE_LIGHT_VALUE      = 0x1008,
            EPIPECmd_VALIDATE_FRAME             = 0x1009,
            EPIPECmd_SET_OPERATION_MODE         = 0x100A,
            EPIPECmd_SET_EFFECT                 = 0x100B,
            EPIPECmd_SET_ZOOM_RATIO             = 0x100C,
            EPIPECmd_SET_BRIGHTNESS             = 0x100D,
            EPIPECmd_SET_CONTRAST               = 0x100E,
            EPIPECmd_SET_EDGE                   = 0x100F,
            EPIPECmd_SET_HUE                    = 0x1010,
            EPIPECmd_SET_SATURATION             = 0x1011,
            EPIPECmd_SEND_TUNING_CMD            = 0x1012,
            EPIPECmd_DECIDE_OFFLINE_CAPTURE     = 0x1013,
            EPIPECmd_LOCK_REG                   = 0x1014,
            EPIPECmd_SET_SHADING_IDX            = 0x1018,

            EPIPECmd_SET_RRZ                    = 0x101A,
            EPIPECmd_SET_P1_UPDATE              = 0x101B,
            EPIPECmd_SET_IMGO                   = 0x101C,

            EPIPECmd_SET_BASE_ADDR              = 0x1102,
            EPIPECmd_SET_CQ_CHANNEL             = 0x1103,
            EPIPECmd_SET_CQ_TRIGGER_MODE        = 0x1104,
            EPIPECmd_AE_SMOOTH                  = 0x1105,
            EPIPECmd_SET_FMT_EN                 = 0x1107,
            EPIPECmd_SET_GDMA_LINK_EN           = 0x1108,
            EPIPECmd_SET_FMT_START              = 0x1109,
            EPIPECmd_SET_CAM_CTL_DBG            = 0x110A,
            EPIPECmd_SET_IMG_PLANE_BY_IMGI      = 0x110B,
            EPIPECmd_SET_P2_QUEUE_CONTROL_STATE = 0x110C,

            EPIPECmd_GET_TG_OUT_SIZE            = 0x110F,
            EPIPECmd_GET_RMX_OUT_SIZE           = 0x1110,
            EPIPECmd_GET_HBIN_INFO              = 0x1111,
            EPIPECmd_GET_EIS_INFO               = 0x1112,
            EPIPECmd_GET_SUGG_BURST_QUEUE_NUM   = 0x1114,

            EPIPECmd_GET_TWIN_INFO              = 0x1116,

            EPIPECmd_SET_EIS_CBFP               = 0X1117,
            EPIPECmd_SET_LCS_CBFP               = 0X1118,
            EPIPECmd_SET_SGG2_CBFP              = 0X1119,

            EPIPECmd_GET_CUR_FRM_STATUS         = 0x111D,
            EPIPECmd_GET_CUR_SOF_IDX            = 0x111E,

            EPIPECmd_ALLOC_UNI                  = 0x1200,
            EPIPECmd_DEALLOC_UNI                = 0x1201,
            //EPIPECmd_ALLOC_FLK_PATH             = 0x1200,
            //EPIPECmd_DEALLOC_FLK_PATH           = 0x1201,
            //EPIPECmd_ALLOC_HDS_PATH             = 0x1202,
            //EPIPECmd_DEALLOC_HDS_PATH           = 0x1203,

            EPIPECmd_SET_NR3D_EN                = 0x1300,
            EPIPECmd_SET_NR3D_DMA_SEL           = 0x1301,
            EPIPECmd_SET_CRZ_EN                 = 0x1302,
            EPIPECmd_SET_JPEG_CFG               = 0x1303,
            EPIPECmd_SET_JPEG_WORKBUF_SIZE      = 0x1304,

            EPIPECmd_SET_MODULE_EN              = 0x1401, //phase out
            EPIPECmd_SET_MODULE_SEL             = 0x1402, //phase out
            EPIPECmd_SET_MODULE_CFG             = 0x1403, //phase out
            EPIPECmd_GET_MODULE_HANDLE          = 0x1404, //phase out
            EPIPECmd_SET_MODULE_CFG_DONE        = 0x1405, //phase out
            EPIPECmd_RELEASE_MODULE_HANDLE      = 0x1406, //phase out
            EPIPECmd_SET_MODULE_DBG_DUMP        = 0x1407, //phase out


            EPIPECmd_GET_SENSOR_PRV_RANGE       = 0x2001,
            EPIPECmd_GET_SENSOR_FULL_RANGE      = 0x2002,
            EPIPECmd_GET_RAW_DUMMY_RANGE        = 0x2003,
            EPIPECmd_GET_SENSOR_NUM             = 0x2004,
            EPIPECmd_GET_SENSOR_TYPE            = 0x2005,
            EPIPECmd_GET_RAW_INFO               = 0x2006,
            EPIPECmd_GET_EXIF_DEBUG_INFO        = 0x2007,
            EPIPECmd_GET_SHADING_IDX            = 0x2008,
            EPIPECmd_GET_ATV_DISP_DELAY         = 0x2009,
            EPIPECmd_GET_SENSOR_DELAY_FRAME_CNT = 0x200A,
            EPIPECmd_GET_CAM_CTL_DBG            = 0x200B,
            EPIPECmd_GET_FMT                    = 0x200C,
            EPIPECmd_GET_GDMA                   = 0x200D,
            EPIPECmd_GET_NR3D_GAIN              = 0x200E,
            EPIPECmd_ISP_RESET                  = 0x4001,
            EPIPECmd_MAX                        = 0xFFFF

        };
    };
};
#endif

using namespace NS3A;
using namespace NS3Av3;
using namespace NSIspTuning;
using namespace NSIspTuningv3;
using namespace NSCam;
using namespace NSIoPipe;
using namespace NSCamIOPipe;

AwbMgr* AwbMgr::s_pAwbMgr = MNULL;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template <ESensorDev_T const eSensorDev>
class AwbMgrDev : public AwbMgr
{
public:
    static
        AwbMgr&
        getInstance()
    {
        static AwbMgrDev<eSensorDev> singleton;
        AwbMgr::s_pAwbMgr = &singleton;
        return singleton;
    }

    AwbMgrDev()
        : AwbMgr(eSensorDev)
    {}

    virtual ~AwbMgrDev() {}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AwbMgr&
    AwbMgr::
    getInstance(MINT32 const i4SensorDev)
{
    switch (i4SensorDev)
    {
    case ESensorDev_Main: //  Main Sensor
        return  AwbMgrDev<ESensorDev_Main>::getInstance();
    case ESensorDev_MainSecond: //  Main Second Sensor
        return  AwbMgrDev<ESensorDev_MainSecond>::getInstance();
    case ESensorDev_Sub: //  Sub Sensor
        return  AwbMgrDev<ESensorDev_Sub>::getInstance();
    default:
        MY_LOG("i4SensorDev = %d", i4SensorDev);
        if (AwbMgr::s_pAwbMgr)
            return  *AwbMgr::s_pAwbMgr;
        else
            return  AwbMgrDev<ESensorDev_Main>::getInstance();
    }
}



static void aaoSeparation(void* aao, int w, int h, void* awb, int awbPitch, void* hist)
{
    int i;
    int j;
    MUINT8* pawb=(MUINT8*)awb;
    MUINT8* p=(MUINT8*)aao;

    int lineByte;
    lineByte = (int)(w*8.5+0.5);

    // Fill awb
    if(awb!=0)
    {
        int awbW=w*4;
        if(awbPitch>awbW)
            awbW=awbPitch;
        for(j=0;j<90;j++)
        {
            memcpy(pawb, p, w*4);
            p+=lineByte;
            pawb+=awbW;
        }
    }

    // Fill hist
    if(hist!=0)
    {
        int* pHist=(int* )hist;
        p=(MUINT8*)aao;
        int aaoNoHistSz;
        aaoNoHistSz = lineByte*h;
        p+=aaoNoHistSz;
        for(j=0;j<4;j++)
        {
            for(i=0;i<128;i++)
            {
                int v;
                int b1;
                int b2;
                int b3;
                b1 = *p;
                b2 = *(p+1);
                b3 = *(p+2);
                v = b1+(b2<<8)+(b3<<16);
                p+=3;
                *pHist=v;
                pHist++;
            }
        }
    }


}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AwbMgr::
    AwbMgr(ESensorDev_T eSensorDev)
#if USE_OPEN_SOURCE_AWB
    : m_pIAwbAlgo(IAwbAlgo::createInstance<EAAAOpt_OpenSource>(eSensorDev))
#else
    : m_pIAwbAlgo(IAwbAlgo::createInstance<EAAAOpt_MTK>(eSensorDev))
#endif
    , m_pAwbStateMgr(AwbStateMgr::getInstance(eSensorDev))
    , m_eAWBMode(LIB3A_AWB_MODE_AUTO)
    , m_i4StrobeMode(AWB_STROBE_MODE_OFF)
    , m_i4AWBNvramIdx(AWB_NVRAM_IDX_NORMAL)
    , m_bEnableAWB(MFALSE)
    , m_bAWBLock(MFALSE)
    , m_bAdbAWBLock(MFALSE)
    , m_bOneShotAWB(MFALSE)
    , m_bAWBModeChanged(MFALSE)
    , m_bStrobeModeChanged(MFALSE)
    , m_bColdBoot(MTRUE)
    , m_bAWBNvramIdxChanged(MFALSE)
    , m_eSensorDev(eSensorDev)
    , m_eSensorTG(ESensorTG_1)
    , m_eSensorMode(ESensorMode_Preview)
    , m_i4SensorIdx(0)
    , m_bDebugEnable(MFALSE)
    , m_bInitState(MFALSE)
    , m_i4AFLV(70)
    , m_bSkipOneFrame(MFALSE)
    , m_bHBIN2Enable(MFALSE)
    , m_bAWBCalibrationBypassed(MFALSE)
    , m_pNVRAM_3A(MNULL)
    , m_flashAwbWeight(0)
    , m_flashDuty(0)
    , m_flashStep(0)
    , m_rAWBStatParam()
    , m_rAWBWindowConfig()
	, mIsMono(0)
    , m_eColorCorrectionMode(MTK_COLOR_CORRECTION_MODE_FAST)
{
    mBackupGain.i4R=512;
    mBackupGain.i4G=512;
    mBackupGain.i4B=512;
    m_rColorCorrectionGain.i4R = m_rColorCorrectionGain.i4G = m_rColorCorrectionGain.i4B = 512;

    switch (eSensorDev)
    {
    case ESensorDev_Main: //  Main Sensor
        m_bEnableAWB = isAWBEnabled<ESensorDev_Main>();
        break;
    case ESensorDev_MainSecond: //  Main Second Sensor
        m_bEnableAWB = isAWBEnabled<ESensorDev_MainSecond>();
        break;
    case ESensorDev_Sub: //  Sub Sensor
        m_bEnableAWB = isAWBEnabled<ESensorDev_Sub>();
        break;
    default:
        MY_ERR("eSensorDev = %d", eSensorDev);
        break;
    }
    mpAao = new MUINT8[54000];
    mpHist = new MUINT8[2048];

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AwbMgr::
    ~AwbMgr()
{
    delete []mpAao;
    delete []mpHist;

}

#include <cutils/properties.h>
static int getPropInt(const char* sId, int DefVal)
{
	int ret;
	char ss[PROPERTY_VALUE_MAX] = {'\0'};
	char sDef[20];
	sprintf(sDef,"%d",DefVal);
	property_get(sId, ss, sDef);
	ret = atoi(ss);
	return ret;
}



#define setRgbGainX(a, b)  { setRgbGain(a,b);}
// #define setRgbGainX(a, b)  { MY_LOG("setRgbGainX ln=%d",__LINE__); setRgbGain(a,b);}
MBOOL AwbMgr::setRgbGain(MINT32 const i4SensorDev, AWB_GAIN_T& rIspAWBGain)
{
    int propR;
    int propG;
    int propB;
    propR = getPropInt("z.awb_gain_r", -1);
    propG = getPropInt("z.awb_gain_g", -1);
    propB = getPropInt("z.awb_gain_b", -1);
    if((propR!=-1) && (propG!=-1) && (propB!=-1))
    {
      rIspAWBGain.i4R = propR;
      rIspAWBGain.i4G = propG;
      rIspAWBGain.i4B = propG;
    }

	if(mIsMono)
	{
		rIspAWBGain.i4R=512;
		rIspAWBGain.i4G=512;
		rIspAWBGain.i4B=512;
	}

    strSensorAWBGain rSensorAWBGain;
    rSensorAWBGain.u4R = rIspAWBGain.i4R;
    rSensorAWBGain.u4GR = rIspAWBGain.i4G;
    rSensorAWBGain.u4GB = rIspAWBGain.i4G;
    rSensorAWBGain.u4B = rIspAWBGain.i4B;

    MY_LOG("awb gain %d %d %d %d",i4SensorDev, rIspAWBGain.i4R,rIspAWBGain.i4G, rIspAWBGain.i4B);

    //AAASensorMgr::getInstance().setSensorAWBGain((ESensorDev_T)i4SensorDev, &rSensorAWBGain);


    return IspTuningMgr::getInstance().setAWBGain(i4SensorDev, rIspAWBGain);

}


MBOOL AwbMgr::setAEPreGain2(MINT32 const i4SensorDev, MINT32  sensorIdx, AWB_GAIN_T& rPreGain2)
{
  if(mIsMono)
  {
		rPreGain2.i4R=512;
		rPreGain2.i4G=512;
		rPreGain2.i4B=512;
	}
  IspTuningMgr::getInstance().setIspAEPreGain2(i4SensorDev, sensorIdx, rPreGain2);
  return 1;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void printGain(AWB_GAIN_T& gain)
{
    int r;
    int g;
    int b;
    r = gain.i4R;
    g = gain.i4G;
    b = gain.i4B;
    static int vv=0;
    vv++;
    MY_LOG("printGain 0 t=%d rgb=%d %d %d",vv,r,g,b);
}

MBOOL AwbMgr::cameraPreviewInit(MINT32 i4SensorIdx, Param_T &rParam, IAWBInitPara* initPara)
{
    MY_LOG("[%s()] m_eSensorDev: %d, i4SensorIdx = %d, m_eSensorMode = %d\n", __FUNCTION__, m_eSensorDev, i4SensorIdx, m_eSensorMode);

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("debug.awb_mgr.enable", value, "0");
    m_bDebugEnable = atoi(value);

    // sensor index
    m_i4SensorIdx = i4SensorIdx;

    // set strobe mode to OFF
    setStrobeMode(AWB_STROBE_MODE_OFF);

    // Get sensor resolution
    getSensorResolution();

    // Get NVRAM data
    if (!getNvramData()) {
        MY_ERR("getNvramData() fail\n");
        return MFALSE;
    }

    // Init AWB
    if (!AWBInit()) {
        MY_ERR("AWBInit() fail\n");
        return MFALSE;
    }

    // AWB statistics config
    ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[m_i4StrobeMode][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable
        , mAaoW, mAaoH, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY);

    // update AE RAW pre-gain2
    setAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);
    // IspTuningMgr::getInstance().setIspAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);

    // update AWB gain
    //  IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);
    setRgbGainX(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);

    // force ISP validate
    IspTuningMgr::getInstance().forceValidate(m_eSensorDev);

    return MTRUE;
}

MBOOL AwbMgr::init(MINT32 i4SensorIdx, IAWBInitPara* initPara)
{
    MY_LOG("[%s()] m_eSensorDev: %d, i4SensorIdx = %d, m_eSensorMode = %d\n", __FUNCTION__, m_eSensorDev, i4SensorIdx, m_eSensorMode);

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("debug.awb_mgr.enable", value, "0");
    m_bDebugEnable = atoi(value);

    // sensor index
    m_i4SensorIdx = i4SensorIdx;



    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::camcorderPreviewInit(MINT32 i4SensorIdx, Param_T &rParam, IAWBInitPara* initPara)
{
    MY_LOG("[%s()] m_eSensorDev: %d, i4SensorIdx = %d, m_eSensorMode = %d\n", __FUNCTION__, m_eSensorDev, i4SensorIdx, m_eSensorMode);

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("debug.awb_mgr.enable", value, "0");
    m_bDebugEnable = atoi(value);

    // sensor index
    m_i4SensorIdx = i4SensorIdx;

    MY_LOG("rParam.u4StrobeMode = %d\n", rParam.u4StrobeMode);
    if (rParam.u4StrobeMode == LIB3A_FLASH_MODE_FORCE_TORCH) // Torch mode
        setStrobeMode(AWB_STROBE_MODE_ON);
    else
        setStrobeMode(AWB_STROBE_MODE_OFF);

    // Get sensor resolution
    getSensorResolution();

    // Get NVRAM data
    if (!getNvramData()) {
        MY_ERR("getNvramData() fail\n");
        return MFALSE;
    }

    // AWB init
    if (!AWBInit()) {
        MY_ERR("AWBInit() fail\n");
        return MFALSE;
    }

    // AWB statistics config
    ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[m_i4StrobeMode][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable
        , mAaoW, mAaoH, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY);

    // update AE RAW pre-gain2
    setAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);
    // IspTuningMgr::getInstance().setIspAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);

    // update AWB gain
    //  IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);
    setRgbGainX(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);

    // force ISP validate
    IspTuningMgr::getInstance().forceValidate(m_eSensorDev);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::cameraCaptureInit(IAWBInitPara* initPara)
{
    MY_LOG("[%s()] m_eSensorDev: %d\n", __FUNCTION__, m_eSensorDev);

    queryHBinInfo();

    if (m_bEnableAWB && (m_eAWBMode != LIB3A_AWB_MODE_OFF))
    {
        if (m_i4StrobeMode == AWB_STROBE_MODE_ON) {
            m_pIAwbAlgo->setAWBStatConfig(m_rAWBStatCfg[AWB_STROBE_MODE_ON][m_eSensorMode][m_eAWBMode],
                m_rAWBWindowConfig.i4SensorWidth[m_eSensorMode],
                m_rAWBWindowConfig.i4SensorHeight[m_eSensorMode]);

            // update AWB statistics config
            ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[AWB_STROBE_MODE_ON][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable
                , mAaoW, mAaoH, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY);
        }
        else {
            m_pIAwbAlgo->setAWBStatConfig(m_rAWBStatCfg[AWB_STROBE_MODE_OFF][m_eSensorMode][m_eAWBMode],
                m_rAWBWindowConfig.i4SensorWidth[m_eSensorMode],
                m_rAWBWindowConfig.i4SensorHeight[m_eSensorMode]);

            // update AWB statistics config
            ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[AWB_STROBE_MODE_OFF][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable
                , mAaoW, mAaoH, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY);
        }

        // force ISP validate
        IspTuningMgr::getInstance().forceValidate(m_eSensorDev);
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::cameraPreviewReinit(IAWBInitPara* initPara)
{
    MY_LOG("[%s()] m_eSensorDev: %d\n", __FUNCTION__, m_eSensorDev);

    // set strobe mode to OFF: temp added
    setStrobeMode(AWB_STROBE_MODE_OFF);

    m_bStrobeModeChanged = MFALSE;
    m_bAWBModeChanged = MFALSE;
    m_bOneShotAWB = MTRUE; // do one-shot AWB
    m_bInitState = MTRUE; // init state

    // AWB statistics config
    ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[AWB_STROBE_MODE_OFF][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable
        , mAaoW, mAaoH, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY);

    // update AE RAW pre-gain2
    setAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);
    // IspTuningMgr::getInstance().setIspAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);

    // update AWB gain
    //  IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);
    setRgbGainX(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);

    // force ISP validate
    IspTuningMgr::getInstance().forceValidate(m_eSensorDev);

    return MTRUE;
}
MBOOL AwbMgr::start()
{//capture init/ preview reinit
    MY_LOG("start ln=%d senDev=%d",__LINE__,m_eSensorDev);

    // set strobe mode to OFF
    setStrobeMode(AWB_STROBE_MODE_OFF);

    // Get sensor resolution
    getSensorResolution();

    // Get NVRAM data
    if (!getNvramData()) {
        MY_ERR("getNvramData() fail\n");
        return MFALSE;
    }

    // Init AWB
    if (!AWBInit()) {
        MY_ERR("AWBInit() fail\n");
        return MFALSE;
    }

    // AWB statistics config
    //ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[m_i4StrobeMode][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable);

    // update AE RAW pre-gain2
    //IspTuningMgr::getInstance().setIspAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);

    // update AWB gain
    //IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);

    // force ISP validate
    //IspTuningMgr::getInstance().forceValidate(m_eSensorDev);

    queryHBinInfo();
    m_bStrobeModeChanged = MFALSE;
    m_bAWBModeChanged = MFALSE;
    m_bOneShotAWB = MTRUE; // do one-shot AWB
    m_bInitState = MTRUE; // init state

    if (m_i4StrobeMode == AWB_STROBE_MODE_ON) {
        m_pIAwbAlgo->setAWBStatConfig(m_rAWBStatCfg[AWB_STROBE_MODE_ON][m_eSensorMode][m_eAWBMode],
            m_rAWBWindowConfig.i4SensorWidth[m_eSensorMode],
            m_rAWBWindowConfig.i4SensorHeight[m_eSensorMode]);

        // update AWB statistics config
        ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[AWB_STROBE_MODE_ON][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable
            , mAaoW, mAaoH, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY);
    }
    else {
        m_pIAwbAlgo->setAWBStatConfig(m_rAWBStatCfg[AWB_STROBE_MODE_OFF][m_eSensorMode][m_eAWBMode],
            m_rAWBWindowConfig.i4SensorWidth[m_eSensorMode],
            m_rAWBWindowConfig.i4SensorHeight[m_eSensorMode]);

        // update AWB statistics config
        ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[AWB_STROBE_MODE_OFF][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable
            , mAaoW, mAaoH, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY);
    }

    // update AE RAW pre-gain2
    IspTuningMgr::getInstance().setIspAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);

    // update AWB gain
    IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);
    IspTuningMgr::getInstance().setAWBInfo(m_eSensorDev, m_rAWBOutput.rAWBInfo);

    // force ISP validate
    IspTuningMgr::getInstance().forceValidate(m_eSensorDev);
    return MTRUE;
}
MBOOL AwbMgr::stop()
{
    MY_LOG("stop ln=%d senDev=%d",__LINE__,m_eSensorDev);
    return MTRUE;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::uninit()
{
    MY_LOG("[%s()] m_eSensorDev: %d\n", __FUNCTION__, m_eSensorDev);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::setAWBMode(MINT32 i4NewAWBMode)
{
    LIB3A_AWB_MODE_T eNewAWBMode;
    MY_LOG("[%s()] m_eSensorDev: %d mode=%d\n", __FUNCTION__, m_eSensorDev, i4NewAWBMode);
    //MY_LOG("i4NewAWBMode: %d\n", i4NewAWBMode);

    switch (i4NewAWBMode) {
    case MTK_CONTROL_AWB_MODE_OFF: // Off
        eNewAWBMode = LIB3A_AWB_MODE_OFF;
        break;
    case MTK_CONTROL_AWB_MODE_AUTO: // Auto
        eNewAWBMode = LIB3A_AWB_MODE_AUTO;
        break;
    case MTK_CONTROL_AWB_MODE_DAYLIGHT: // Daylight
        eNewAWBMode = LIB3A_AWB_MODE_DAYLIGHT;
        break;
    case MTK_CONTROL_AWB_MODE_CLOUDY_DAYLIGHT: // Cloudy daylight
        eNewAWBMode = LIB3A_AWB_MODE_CLOUDY_DAYLIGHT;
        break;
    case MTK_CONTROL_AWB_MODE_SHADE: // Shade
        eNewAWBMode = LIB3A_AWB_MODE_SHADE;
        break;
    case MTK_CONTROL_AWB_MODE_TWILIGHT: // Twilight
        eNewAWBMode = LIB3A_AWB_MODE_TWILIGHT;
        break;
    case MTK_CONTROL_AWB_MODE_FLUORESCENT: // Fluorescent
        eNewAWBMode = LIB3A_AWB_MODE_FLUORESCENT;
        break;
    case MTK_CONTROL_AWB_MODE_WARM_FLUORESCENT: // Warm fluorescent
        eNewAWBMode = LIB3A_AWB_MODE_WARM_FLUORESCENT;
        break;
    case MTK_CONTROL_AWB_MODE_INCANDESCENT: // Incandescent
        eNewAWBMode = LIB3A_AWB_MODE_INCANDESCENT;
        break;
    case MTK_CONTROL_AWB_MODE_GRAYWORLD: // Grayword
        eNewAWBMode = LIB3A_AWB_MODE_GRAYWORLD;
        break;
    default:
        MY_ERR("E_AWB_UNSUPPORT_MODE: %d\n", i4NewAWBMode);
        return (E_AWB_UNSUPPORT_MODE);
    }

    if (m_eAWBMode != eNewAWBMode)
    {
        m_eAWBMode = eNewAWBMode;
        m_bAWBModeChanged = MTRUE;

        // reset AWB state
        if (m_eAWBMode == LIB3A_AWB_MODE_AUTO)
            m_pAwbStateMgr->reset(eState_InactiveAuto);
        else
            m_pAwbStateMgr->reset(eState_InactiveNotAuto);

        MY_LOG("m_eAWBMode: %d\n", m_eAWBMode);
    }



    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AwbMgr::getAWBMode() const
{
    return static_cast<MINT32>(m_eAWBMode);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::setSensorMode(MINT32 i4NewSensorMode, int w, int h)
{
    ESensorMode_T eNewSensorMode;
    MY_LOG("[%s()] m_eSensorDev: %d\n", __FUNCTION__, m_eSensorDev);
    MY_LOG("i4NewSensorMode: %d\n", i4NewSensorMode);

    switch (i4NewSensorMode) {
    case SENSOR_SCENARIO_ID_NORMAL_PREVIEW:
        eNewSensorMode = ESensorMode_Preview;
        break;
    case SENSOR_SCENARIO_ID_NORMAL_CAPTURE:
        eNewSensorMode = ESensorMode_Capture;
        break;
    case SENSOR_SCENARIO_ID_NORMAL_VIDEO:
        eNewSensorMode = ESensorMode_Video;
        break;
    case SENSOR_SCENARIO_ID_SLIM_VIDEO1:
        eNewSensorMode = ESensorMode_SlimVideo1;
        break;
    case SENSOR_SCENARIO_ID_SLIM_VIDEO2:
        eNewSensorMode = ESensorMode_SlimVideo2;
        break;
    case SENSOR_SCENARIO_ID_CUSTOM1:
        eNewSensorMode = ESensorMode_Custom1;
        break;
    case SENSOR_SCENARIO_ID_CUSTOM2:
        eNewSensorMode = ESensorMode_Custom2;
        break;
    case SENSOR_SCENARIO_ID_CUSTOM3:
        eNewSensorMode = ESensorMode_Custom3;
        break;
    case SENSOR_SCENARIO_ID_CUSTOM4:
        eNewSensorMode = ESensorMode_Custom4;
        break;
    case SENSOR_SCENARIO_ID_CUSTOM5:
        eNewSensorMode = ESensorMode_Custom5;
        break;
    default:
        MY_ERR("E_AWB_UNSUPPORT_SENSOR_MODE: %d\n", i4NewSensorMode);
        return MFALSE;
    }

    if ((m_eSensorMode != eNewSensorMode) || (mAaoW != w) || (mAaoH = h))
    {
        m_eSensorMode = eNewSensorMode;
        mAaoW = w;
        mAaoH = h;
        MY_LOG("m_eSensorMode: %d\n w=%d h=%d", m_eSensorMode,w,h);
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AwbMgr::getSensorMode() const
{
    return static_cast<MINT32>(m_eSensorMode);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::setStrobeMode(MINT32 i4NewStrobeMode)
{
    if ((i4NewStrobeMode < AWB_STROBE_MODE_ON) || (i4NewStrobeMode > AWB_STROBE_MODE_OFF))
    {
        MY_ERR("Unsupport strobe mode: %d\n", i4NewStrobeMode);
        return E_AWB_UNSUPPORT_MODE;
    }

    if (m_i4StrobeMode != i4NewStrobeMode)
    {
        m_i4StrobeMode = i4NewStrobeMode;
        m_bStrobeModeChanged = MTRUE;
        MY_LOG("[%d] m_i4StrobeMode: %d\n", m_eSensorDev, m_i4StrobeMode);
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AwbMgr::getStrobeMode() const
{
    return m_i4StrobeMode;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::setFlashAWBData(FLASH_AWB_PASS_FLASH_INFO_T &rFlashAwbData)
{
    m_flashAwbWeight = rFlashAwbData.flashAwbWeight;
    m_flashDuty = rFlashAwbData.flashDuty;
    m_flashStep = rFlashAwbData.flashStep;

    MY_LOG(" [%d] m_flashAwbWeight = %d\n", m_eSensorDev, m_flashAwbWeight);
    MY_LOG(" [%d] m_flashDuty = %d\n", m_eSensorDev, m_flashDuty);
    MY_LOG(" [%d] m_flashStep = %d\n", m_eSensorDev, m_flashStep);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::setAWBLock(MBOOL bAWBLock)
{
    MY_LOG("[%d] bAWBLock: %d\n", m_eSensorDev, bAWBLock);

    if (m_bAWBLock != bAWBLock)
    {
        if (bAWBLock) { // AWB lock
            m_bAWBLock = MTRUE;
            m_bOneShotAWB = MTRUE;
            m_pAwbStateMgr->sendIntent(eIntent_AWBLockOn);
        }
        else { // AWB unlock
            m_bAWBLock = MFALSE;
            m_pAwbStateMgr->sendIntent(eIntent_AWBLockOff);
        }

        MY_LOG("[%d] m_bAWBLock: %d\n", m_eSensorDev, m_bAWBLock);
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::enableAWB()
{
    m_bEnableAWB = MTRUE;

    MY_LOG("[%d] enableAWB()\n", m_eSensorDev);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::disableAWB()
{
    m_bEnableAWB = MFALSE;

    MY_LOG("[%d] disableAWB()\n", m_eSensorDev);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AwbMgr::backup()
{
    mBackupGain = m_rAWBOutput.rPreviewAWBGain;
    //  printGain(mBackupGain);
    return MTRUE;
}
MRESULT AwbMgr::restore()
{
    m_rAWBOutput.rPreviewAWBGain = mBackupGain;
    m_rAWBOutput.rAWBInfo.rCurrentAWBGain = mBackupGain;
    //mBackupGain = m_rAWBOutput.rPreviewAWBGain;
    IspTuningMgr::getInstance().setAWBGain(m_eSensorDev,mBackupGain);
    IspTuningMgr::getInstance().setAWBInfo(m_eSensorDev,m_rAWBOutput.rAWBInfo);

    //  printGain(mBackupGain);
    return MTRUE;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::setAWBStatCropRegion(MINT32 i4SensorMode, MINT32 i4CropOffsetX, MINT32 i4CropOffsetY, MINT32 i4CropRegionWidth, MINT32 i4CropRegionHeight)
{
    m_rAWBWindowConfig.bNeedCrop[i4SensorMode] = MTRUE;
    m_rAWBWindowConfig.i4CropOffsetX[i4SensorMode] = i4CropOffsetX;
    m_rAWBWindowConfig.i4CropOffsetY[i4SensorMode] = i4CropOffsetY;
    m_rAWBWindowConfig.i4CropRegionWidth[i4SensorMode] = i4CropRegionWidth;
    m_rAWBWindowConfig.i4CropRegionHeight[i4SensorMode] = i4CropRegionHeight;

    MY_LOG("[%s()][%d] i4SensorMode = %d, bNeedCrop = %d, i4CropOffsetX = %d, i4CropOffsetY = %d, i4CropRegionWidth = %d, i4CropRegionHeight = %d",
            __FUNCTION__,
            m_eSensorDev, i4SensorMode,
            m_rAWBWindowConfig.bNeedCrop[i4SensorMode],
            m_rAWBWindowConfig.i4CropOffsetX[i4SensorMode],
            m_rAWBWindowConfig.i4CropOffsetY[i4SensorMode],
            m_rAWBWindowConfig.i4CropRegionWidth[i4SensorMode],
            m_rAWBWindowConfig.i4CropRegionHeight[i4SensorMode]);

    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AwbMgr::doPvAWB(MINT32 i4FrameCount, MBOOL bAEStable, MINT32 i4SceneLV, MVOID *aaoBuf, MBOOL bApplyToHW)
{
    //use setprop to decide AWB is controlled by Sync AWB algo or AWB mgr
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("debug.awb_mgr.lock", value, "0");
    m_bAdbAWBLock = atoi(value);

    AWB_INPUT_T rAWBInput;

    MVOID* pAWBStatBuf;
    pAWBStatBuf = mpAao;
    aaoSeparation(aaoBuf, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY, mpAao, m_rAWBStatParam.i4WindowNumX*5, mpHist);


    MY_LOG("doPvAWB m_eColorCorrectionMode=%d ln=%d", m_eColorCorrectionMode, __LINE__);
    if (m_eColorCorrectionMode == MTK_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX)
    {
        MY_LOG("doPvAWB ln=%d", __LINE__);
        MY_LOG("rgain=%d ggain=%d bgain=%d",
            m_rColorCorrectionGain.i4R,
            m_rColorCorrectionGain.i4G,
            m_rColorCorrectionGain.i4B);
        m_rAWBOutput.rAWBInfo.rCurrentAWBGain = m_rColorCorrectionGain;
        IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rAWBInfo.rCurrentAWBGain);
        IspTuningMgr::getInstance().setAWBInfo(m_eSensorDev, m_rAWBOutput.rAWBInfo);
    }
    else if ((m_bEnableAWB) &&
        (!m_bAWBLock) &&
        (!m_bAdbAWBLock) &&
        (m_eAWBMode != LIB3A_AWB_MODE_OFF))
    {
        //if ((m_bAWBModeChanged) && (!m_bInitState))
        if ((m_bAWBModeChanged))
        {
            m_pIAwbAlgo->setAWBMode(m_eAWBMode);
            m_pIAwbAlgo->setAWBStatConfig(m_rAWBStatCfg[AWB_STROBE_MODE_OFF][m_eSensorMode][m_eAWBMode],
                m_rAWBWindowConfig.i4SensorWidth[m_eSensorMode],
                m_rAWBWindowConfig.i4SensorHeight[m_eSensorMode]);
            m_bAWBModeChanged = MFALSE;
            m_bOneShotAWB = MTRUE;
            m_bSkipOneFrame = MTRUE;

            // update AWB statistics config
            ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[AWB_STROBE_MODE_OFF][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable
                , mAaoW, mAaoH, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY);

            // force ISP validate
            IspTuningMgr::getInstance().forceValidate(m_eSensorDev);
        }
        else if ((m_bStrobeModeChanged) && (!m_bInitState))
        {
            MY_LOG("[%d] m_bStrobeModeChanged = %d, m_i4StrobeMode = %d\n", m_eSensorDev, m_bStrobeModeChanged, m_i4StrobeMode);
            MY_LOG("[%s()][%d] update AWB statistics config", __FUNCTION__, m_eSensorDev);

            if (m_i4StrobeMode == AWB_STROBE_MODE_ON) {
                m_pIAwbAlgo->setAWBStatConfig(m_rAWBStatCfg[AWB_STROBE_MODE_ON][m_eSensorMode][m_eAWBMode],
                    m_rAWBWindowConfig.i4SensorWidth[m_eSensorMode],
                    m_rAWBWindowConfig.i4SensorHeight[m_eSensorMode]);
                // update AWB statistics config
                ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[AWB_STROBE_MODE_ON][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable
                    , mAaoW, mAaoH, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY);
            }
            else {
                m_pIAwbAlgo->setAWBStatConfig(m_rAWBStatCfg[AWB_STROBE_MODE_OFF][m_eSensorMode][m_eAWBMode],
                    m_rAWBWindowConfig.i4SensorWidth[m_eSensorMode],
                    m_rAWBWindowConfig.i4SensorHeight[m_eSensorMode]);
                // update AWB statistics config
                ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[AWB_STROBE_MODE_OFF][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable
                    , mAaoW, mAaoH, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY);
            }

            // force ISP validate
            IspTuningMgr::getInstance().forceValidate(m_eSensorDev);

            m_bStrobeModeChanged = MFALSE;
            m_bOneShotAWB = MTRUE;
            m_bSkipOneFrame = MTRUE;
        }
        else if (m_bSkipOneFrame) { // skip one frame for AWB statistics ready
            m_bSkipOneFrame = MFALSE;
        }
        else {
            rAWBInput.bIsStrobeFired = (m_i4StrobeMode == AWB_STROBE_MODE_ON) ? MTRUE : MFALSE;
            rAWBInput.i4SceneLV = i4SceneLV;
            rAWBInput.i4AWBState = AWB_STATE_PREVIEW;
            //rAWBInput.pAWBStatBuf = pAWBStatBuf;
            if (m_bOneShotAWB)
            {
                rAWBInput.eAWBSpeedMode = AWB_SPEED_MODE_ONESHOT;

                if (!m_bInitState) {
                    m_bOneShotAWB = MFALSE;
                }
            }
            else
            {
                rAWBInput.eAWBSpeedMode = AWB_SPEED_MODE_SMOOTH_TRANSITION;
            }
            //rAWBInput.i4StatLineSize = m_rAWBStatParam.i4WindowNumX*5;
            rAWBInput.pAWBStatBuf = aaoBuf;
            rAWBInput.i4StatLineSize = 60*17;//120*8.5;
            m_pIAwbAlgo->handleAWB(rAWBInput, m_rAWBOutput);

            if (m_i4StrobeMode == AWB_STROBE_MODE_ON) {
                // update AE RAW pre-gain2
                setAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);
                // IspTuningMgr::getInstance().setIspAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);

                // update AWB gain
                if (bApplyToHW)
                {
                    //  IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rPreviewStrobeAWBGain);
                    setRgbGainX(m_eSensorDev, m_rAWBOutput.rPreviewStrobeAWBGain);
            }
            }
            else {
                // update AE RAW pre-gain2
                setAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);
                // IspTuningMgr::getInstance().setIspAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);

                // update AWB gain
                if (bApplyToHW)
                {
                    //  IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);
                    setRgbGainX(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);
                }

            }

            // set AWB info
            if (bApplyToHW)
                IspTuningMgr::getInstance().setAWBInfo(m_eSensorDev, m_rAWBOutput.rAWBInfo);

            if (m_bInitState && bAEStable) {
                m_bInitState = MFALSE;
            }

            // update AWB state
            if (m_rAWBOutput.rAWBInfo.bAWBStable)
                m_pAwbStateMgr->sendIntent(eIntent_AWBScanFinished);
            else
                m_pAwbStateMgr->sendIntent(eIntent_AWBScan);
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::doAFAWB(MVOID *aaoBuf, MBOOL bApplyToHW)
{
    AWB_INPUT_T rAWBInput;

    MVOID* pAWBStatBuf;
    pAWBStatBuf = mpAao;
    aaoSeparation(aaoBuf, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY, mpAao, m_rAWBStatParam.i4WindowNumX*5, mpHist);

    if (m_eColorCorrectionMode == MTK_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX)
    {
        MY_LOG("doAFAWB ln=%d", __LINE__);
        m_rAWBOutput.rAWBInfo.rCurrentAWBGain = m_rColorCorrectionGain;
        IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rAWBInfo.rCurrentAWBGain);
        IspTuningMgr::getInstance().setAWBInfo(m_eSensorDev, m_rAWBOutput.rAWBInfo);
    }
    else if (m_bEnableAWB && (!m_bAWBLock) && (!m_bAdbAWBLock) && (m_eAWBMode != LIB3A_AWB_MODE_OFF))
    {
        if (/*(m_i4StrobeMode == AWB_STROBE_MODE_ON) &&*/ (m_bStrobeModeChanged))
        {
            m_pIAwbAlgo->setAWBStatConfig(m_rAWBStatCfg[AWB_STROBE_MODE_ON][m_eSensorMode][m_eAWBMode],
                m_rAWBWindowConfig.i4SensorWidth[m_eSensorMode],
                m_rAWBWindowConfig.i4SensorHeight[m_eSensorMode]);
            m_bStrobeModeChanged = FALSE;

            // update AWB statistics config
            ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[m_i4StrobeMode/*AWB_STROBE_MODE_ON*/][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable
                , mAaoW, mAaoH, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY);

                ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[AWB_STROBE_MODE_ON][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable
                     , mAaoW, mAaoH, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY);

            // force ISP validate
            IspTuningMgr::getInstance().forceValidate(m_eSensorDev);

            MY_LOG("[%s()][%d] update AWB statistics config", __FUNCTION__, m_eSensorDev);
            //return S_AWB_OK;
        }

        if (m_i4StrobeMode == AWB_STROBE_MODE_ON) {

            rAWBInput.bIsStrobeFired = MTRUE;
            rAWBInput.i4SceneLV = getAFLV();
            rAWBInput.i4AWBState = AWB_STATE_AF;
            //rAWBInput.pAWBStatBuf = pAWBStatBuf;
            rAWBInput.eAWBSpeedMode = AWB_SPEED_MODE_ONESHOT;
            //rAWBInput.i4StatLineSize = m_rAWBStatParam.i4WindowNumX*5;
            rAWBInput.pAWBStatBuf = aaoBuf;
            rAWBInput.i4StatLineSize = 60*17;//120*8.5;
            m_pIAwbAlgo->handleAWB(rAWBInput, m_rAWBOutput);
        }

        if (m_i4StrobeMode == AWB_STROBE_MODE_ON) {
            // update AE RAW pre-gain2
            setAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);
            // IspTuningMgr::getInstance().setIspAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);

            // update AWB gain
            if (bApplyToHW)
            {
                //  IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rPreviewStrobeAWBGain);
                setRgbGainX(m_eSensorDev, m_rAWBOutput.rPreviewStrobeAWBGain);
        }
        }
        else {
            // update AE RAW pre-gain2
            setAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);
            // IspTuningMgr::getInstance().setIspAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);

            // update AWB gain
            if (bApplyToHW)
            {
                //  IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);
                setRgbGainX(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);
        }
        }

        // set AWB info
        if (bApplyToHW)
            IspTuningMgr::getInstance().setAWBInfo(m_eSensorDev, m_rAWBOutput.rAWBInfo);

        // update AWB state
        m_pAwbStateMgr->sendIntent(eIntent_AWBScanFinished);

    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::doPreCapAWB(MINT32 i4SceneLV, MVOID *aaoBuf, MBOOL bApplyToHW)
{
    AWB_INPUT_T rAWBInput;

    MVOID* pAWBStatBuf;
    pAWBStatBuf = mpAao;
    aaoSeparation(aaoBuf, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY, mpAao, m_rAWBStatParam.i4WindowNumX*5, mpHist);

    if (m_eColorCorrectionMode == MTK_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX)
    {
        MY_LOG("doPreCapAWB ln=%d", __LINE__);
        m_rAWBOutput.rAWBInfo.rCurrentAWBGain = m_rColorCorrectionGain;
        IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rAWBInfo.rCurrentAWBGain);
        IspTuningMgr::getInstance().setAWBInfo(m_eSensorDev, m_rAWBOutput.rAWBInfo);
    }
    else if (m_bEnableAWB && (((!m_bAWBLock) && (!m_bAdbAWBLock)) || (m_i4StrobeMode == AWB_STROBE_MODE_ON)) && (m_eAWBMode != LIB3A_AWB_MODE_OFF))
    {
        if ((m_i4StrobeMode == AWB_STROBE_MODE_ON) && (m_bStrobeModeChanged))
        {
            m_pIAwbAlgo->setAWBStatConfig(m_rAWBStatCfg[AWB_STROBE_MODE_ON][m_eSensorMode][m_eAWBMode],
                m_rAWBWindowConfig.i4SensorWidth[m_eSensorMode],
                m_rAWBWindowConfig.i4SensorHeight[m_eSensorMode]);
            m_bStrobeModeChanged = FALSE;

            // update AWB statistics config
            ISP_MGR_AWB_STAT_CONFIG_T::getInstance(m_eSensorDev).config(m_rAWBStatCfg[AWB_STROBE_MODE_ON][m_eSensorMode][m_eAWBMode], m_bHBIN2Enable, mAaoW, mAaoH, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY);


            // force ISP validate
            IspTuningMgr::getInstance().forceValidate(m_eSensorDev);

            MY_LOG("[%s()][%d] update AWB statistics config", __FUNCTION__, m_eSensorDev);
            return S_AWB_OK;
        }

        rAWBInput.bIsStrobeFired = (m_i4StrobeMode == AWB_STROBE_MODE_ON) ? MTRUE : MFALSE;
        rAWBInput.i4SceneLV = i4SceneLV;
        rAWBInput.i4AWBState = AWB_STATE_PRECAPTURE;
        //rAWBInput.pAWBStatBuf = pAWBStatBuf;
        rAWBInput.eAWBSpeedMode = AWB_SPEED_MODE_ONESHOT;
        //rAWBInput.i4StatLineSize = m_rAWBStatParam.i4WindowNumX*5;
        rAWBInput.pAWBStatBuf = aaoBuf;
        rAWBInput.i4StatLineSize = 60*17;//120*8.5;
        m_pIAwbAlgo->handleAWB(rAWBInput, m_rAWBOutput);

        if (m_i4StrobeMode == AWB_STROBE_MODE_ON) {
            // update AE RAW pre-gain2
            setAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);
            // IspTuningMgr::getInstance().setIspAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);

            // update AWB gain
            if (bApplyToHW)
            {
                //IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rPreviewStrobeAWBGain);
                setRgbGainX(m_eSensorDev, m_rAWBOutput.rPreviewStrobeAWBGain);
        }
        }
        else {
            // update AE RAW pre-gain2
            setAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);
            // IspTuningMgr::getInstance().setIspAEPreGain2(m_eSensorDev, m_i4SensorIdx, m_rAWBOutput.rPreviewRAWPreGain2);

            // update AWB gain
            if (bApplyToHW)
            {
                //IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);
                setRgbGainX(m_eSensorDev, m_rAWBOutput.rPreviewAWBGain);
        }
        }

        // set AWB info
        if (bApplyToHW)
            IspTuningMgr::getInstance().setAWBInfo(m_eSensorDev, m_rAWBOutput.rAWBInfo);

        // update AWB state
        m_pAwbStateMgr->sendIntent(eIntent_AWBScanFinished);

    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::doCapAWB(MINT32 i4SceneLV, MVOID *aaoBuf, MBOOL bApplyToHW)
{
    AWB_INPUT_T rAWBInput;


    MVOID* pAWBStatBuf;
    pAWBStatBuf = mpAao;
    aaoSeparation(aaoBuf, m_rAWBStatParam.i4WindowNumX, m_rAWBStatParam.i4WindowNumY, mpAao, m_rAWBStatParam.i4WindowNumX*5, mpHist);

    if (m_eColorCorrectionMode == MTK_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX)
    {
        MY_LOG("doCapAWB ln=%d", __LINE__);
        m_rAWBOutput.rAWBInfo.rCurrentAWBGain = m_rColorCorrectionGain;
        IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rAWBInfo.rCurrentAWBGain);
        IspTuningMgr::getInstance().setAWBInfo(m_eSensorDev, m_rAWBOutput.rAWBInfo);
    }
    else if (m_bEnableAWB && (((!m_bAWBLock) && (!m_bAdbAWBLock)) || (m_i4StrobeMode == AWB_STROBE_MODE_ON)) && (m_eAWBMode != LIB3A_AWB_MODE_OFF))
    {
        rAWBInput.bIsStrobeFired = (m_i4StrobeMode == AWB_STROBE_MODE_ON) ? MTRUE : MFALSE;
        rAWBInput.i4SceneLV = i4SceneLV;
        rAWBInput.i4AWBState = AWB_STATE_CAPTURE;
        //rAWBInput.pAWBStatBuf = pAWBStatBuf;
        rAWBInput.eAWBSpeedMode = AWB_SPEED_MODE_ONESHOT;
        //rAWBInput.i4StatLineSize = m_rAWBStatParam.i4WindowNumX*5;

        rAWBInput.pAWBStatBuf = aaoBuf;
        rAWBInput.i4StatLineSize = 60*17;//120*8.5;
        m_pIAwbAlgo->handleAWB(rAWBInput, m_rAWBOutput);

#if ((!USE_OPEN_SOURCE_AWB) && (!USE_OPEN_SOURCE_FLASH_AE) )
        if(rAWBInput.bIsStrobeFired)
        {
            if(isFlashAWBv2Enabled() &&
                (m_pNVRAM_3A->rFlashAWBNVRAM.rCalibrationData.flashWBGain[m_flashDuty].i4R !=0) &&
                (m_pNVRAM_3A->rFlashAWBNVRAM.rCalibrationData.flashWBGain[m_flashDuty].i4G !=0) &&
                (m_pNVRAM_3A->rFlashAWBNVRAM.rCalibrationData.flashWBGain[m_flashDuty].i4B !=0))
            {
                FLASH_AWB_CAL_GAIN_INPUT_T FlashAwbCalGainInput;
                FLASH_AWB_OUTPUT_T FlashAwbOutput;
                FLASH_AWB_INIT_T FlashAwbInitParam;
                FlashAwbInitParam.flash_awb_tuning_param = m_pNVRAM_3A->rFlashAWBNVRAM.rTuningParam;

                FlashAwbCalGainInput.NoFlashWBGain = m_rAWBOutput.rPreviewAWBGain;
                FlashAwbCalGainInput.PureFlashWBGain = m_pNVRAM_3A->rFlashAWBNVRAM.rCalibrationData.flashWBGain[m_flashDuty];
                FlashAwbCalGainInput.PureFlashWeight = m_flashAwbWeight;
                FlashAwbCalGainInput.FlashPreferenceGain = m_pNVRAM_3A->rFlashAWBNVRAM.rTuningParam.FlashPreferenceGain;

                MY_LOG("[%d][flashAWB 2.1] m_flashDuty=%d, m_flashAwbWeight=%d ", m_eSensorDev, m_flashDuty, m_flashAwbWeight);
                MY_LOG("[%d][flashAWB 2.1] pureFlash Rgain=%d, Ggain=%d, Bgain=%d", m_eSensorDev, FlashAwbCalGainInput.PureFlashWBGain.i4R, FlashAwbCalGainInput.PureFlashWBGain.i4G, FlashAwbCalGainInput.PureFlashWBGain.i4B);
                MY_LOG("[%d][flashAWB 2.1] preview Rgain=%d, Ggain=%d, Bgain=%d", m_eSensorDev, FlashAwbCalGainInput.NoFlashWBGain.i4R, FlashAwbCalGainInput.NoFlashWBGain.i4G, FlashAwbCalGainInput.NoFlashWBGain.i4B);
                MY_LOG("[%d][flashAWB 2.1] preview FlashPreferenceGain= (%d,  %d, %d)\n", m_eSensorDev,
                                        m_pNVRAM_3A->rFlashAWBNVRAM.rTuningParam.FlashPreferenceGain.i4R, m_pNVRAM_3A->rFlashAWBNVRAM.rTuningParam.FlashPreferenceGain.i4G, m_pNVRAM_3A->rFlashAWBNVRAM.rTuningParam.FlashPreferenceGain.i4B);

                m_pIAwbAlgo->Flash_Awb_calResultGain(FlashAwbCalGainInput, FlashAwbOutput);

                MY_LOG("[%d][flashAWB 2.1] In Awb_mgr.cpp - FlashAwbOutput Rgain=%d, Ggain=%d, Bgain=%d",  m_eSensorDev, FlashAwbOutput.rAWBGain.i4R,  FlashAwbOutput.rAWBGain.i4G,  FlashAwbOutput.rAWBGain.i4B);

                m_rAWBOutput.rCaptureAWBGain.i4R = FlashAwbOutput.rAWBGain.i4R;
                m_rAWBOutput.rCaptureAWBGain.i4G = FlashAwbOutput.rAWBGain.i4G;
                m_rAWBOutput.rCaptureAWBGain.i4B = FlashAwbOutput.rAWBGain.i4B;

                m_rAWBOutput.rAWBInfo.rCurrentAWBGain = m_rAWBOutput.rCaptureAWBGain;
            }
        }
#endif
        // Debug
        //m_rAWBOutput.rCaptureAWBGain.i4R = 512;
        //m_rAWBOutput.rCaptureAWBGain.i4G = 1;
        //m_rAWBOutput.rCaptureAWBGain.i4B = 1;

        // update AWB gain
        if (bApplyToHW)
        {
            //  IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rCaptureAWBGain);
            setRgbGainX(m_eSensorDev, m_rAWBOutput.rCaptureAWBGain);
        }


        // set AWB info
        if (bApplyToHW)
            IspTuningMgr::getInstance().setAWBInfo(m_eSensorDev, m_rAWBOutput.rAWBInfo);

    }

    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::applyAWB(AWB_SYNC_OUTPUT_N3D_T &rAWBSyncOutput)
{
    m_rAWBOutput.rAWBInfo.rCurrentAWBGain = rAWBSyncOutput.rAWBGain;
    m_rAWBOutput.rAWBInfo.i4CCT = rAWBSyncOutput.i4CCT;

    // update AWB gain
    //  IspTuningMgr::getInstance().setAWBGain(m_eSensorDev, m_rAWBOutput.rAWBInfo.rCurrentAWBGain);
    setRgbGainX(m_eSensorDev, m_rAWBOutput.rAWBInfo.rCurrentAWBGain);

    // set AWB info: FIXME: debug info update?
    IspTuningMgr::getInstance().setAWBInfo(m_eSensorDev, m_rAWBOutput.rAWBInfo);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::getDebugInfo(AWB_DEBUG_INFO_T &rAWBDebugInfo, AWB_DEBUG_DATA_T &rAWBDebugData)
{
    m_pIAwbAlgo->getDebugInfo(rAWBDebugInfo, rAWBDebugData);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AwbMgr::getAWBCCT()
{
    return m_rAWBOutput.rAWBInfo.i4CCT;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::getASDInfo(AWB_ASD_INFO_T &a_rAWBASDInfo)
{
    m_pIAwbAlgo->getASDInfo(a_rAWBASDInfo);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::getAWBOutput(AWB_OUTPUT_T &a_rAWBOutput)
{
    a_rAWBOutput = m_rAWBOutput;

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::getAWBGain(AWB_GAIN_T& rAwbGain, MINT32& i4ScaleUnit) const
{
    MY_LOG("%s()\n", __FUNCTION__);

    rAwbGain = m_rAWBOutput.rAWBInfo.rCurrentAWBGain;
    i4ScaleUnit = m_rAWBInitInput.rAWBParam.rChipParam.i4AWBGainOutputScaleUnit;

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MRESULT AwbMgr::getAWBParentStat(AWB_PARENT_BLK_STAT_T &a_rAWBParentState,  MINT32 *a_ParentBlkNumX, MINT32 *a_ParentBlkNumY)
{
        m_pIAwbAlgo->getAWBParentStat(a_rAWBParentState, a_ParentBlkNumX, a_ParentBlkNumY);


        return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::getInitInputParam(SYNC_CTC_INIT_PARAM &CCTInitParam, NORMAL_GAIN_INIT_INPUT_PARAM &NoramlGainInitParam)
{
    if  (!m_pNVRAM_3A) {
        MY_ERR("m_pNVRAM_3A = NULL");
        return MFALSE;
    }

    CCTInitParam.i4CCT[eHorizon_Light] = m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCCTEstimation.i4CCT[eHorizon_Light];
    CCTInitParam.i4CCT[eA_Light] = m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCCTEstimation.i4CCT[eA_Light];
    CCTInitParam.i4CCT[eTL84_Light] = m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCCTEstimation.i4CCT[eTL84_Light];
    CCTInitParam.i4CCT[eDNP_Light] = m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCCTEstimation.i4CCT[eDNP_Light];
    CCTInitParam.i4CCT[eD65_Light] = m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCCTEstimation.i4CCT[eD65_Light];

    NoramlGainInitParam.NormalWB_Gain[eHorizon_Light] = m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rLightAWBGain.rHorizon;
    NoramlGainInitParam.NormalWB_Gain[eA_Light] = m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rLightAWBGain.rA;
    NoramlGainInitParam.NormalWB_Gain[eTL84_Light] = m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rLightAWBGain.rTL84;
    NoramlGainInitParam.NormalWB_Gain[eDNP_Light] = m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rLightAWBGain.rDNP;
    NoramlGainInitParam.NormalWB_Gain[eD65_Light] = m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rLightAWBGain.rD65;

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::getSensorResolution()
{
    MY_LOG("[%s()][%d]\n", __FUNCTION__, m_eSensorDev);

    IHalSensorList* const pIHalSensorList = IHalSensorList::get();
    SensorStaticInfo rSensorStaticInfo;
    pIHalSensorList->querySensorStaticInfo(m_eSensorDev, &rSensorStaticInfo);

    /*
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Preview] = rSensorStaticInfo.previewWidth;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Preview] = rSensorStaticInfo.previewHeight;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Capture] = rSensorStaticInfo.captureWidth;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Capture] = rSensorStaticInfo.captureHeight;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Video] = rSensorStaticInfo.videoWidth;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Video] = rSensorStaticInfo.videoHeight;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_SlimVideo1] = rSensorStaticInfo.video1Width;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_SlimVideo1] = rSensorStaticInfo.video1Height;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_SlimVideo2] = rSensorStaticInfo.video2Width;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_SlimVideo2] = rSensorStaticInfo.video2Height;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom1] = rSensorStaticInfo.SensorCustom1Width;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom1] = rSensorStaticInfo.SensorCustom1Height;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom2] = rSensorStaticInfo.SensorCustom2Width;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom2] = rSensorStaticInfo.SensorCustom2Height;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom3] = rSensorStaticInfo.SensorCustom3Width;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom3] = rSensorStaticInfo.SensorCustom3Height;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom4] = rSensorStaticInfo.SensorCustom4Width;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom4] = rSensorStaticInfo.SensorCustom4Height;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom5] = rSensorStaticInfo.SensorCustom5Width;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom5] = rSensorStaticInfo.SensorCustom5Height;
    */

    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Preview]    = mAaoW;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Preview]   = mAaoH;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Capture]    = mAaoW;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Capture]   = mAaoH;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Video]      = mAaoW;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Video]     = mAaoH;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_SlimVideo1] = mAaoW;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_SlimVideo1]= mAaoH;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_SlimVideo2] = mAaoW;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_SlimVideo2]= mAaoH;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom1]    = mAaoW;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom1]   = mAaoH;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom2]    = mAaoW;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom2]   = mAaoH;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom3]    = mAaoW;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom3]   = mAaoH;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom4]    = mAaoW;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom4]   = mAaoH;
    m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom5]    = mAaoW;
    m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom5]   = mAaoH;

    MY_LOG("i4SensorWidth[ESensorMode_Preview] = %d\n", m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Preview]);
    MY_LOG("i4SensorHeight[ESensorMode_Preview] = %d\n", m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Preview]);
    MY_LOG("i4SensorWidth[ESensorMode_Capture] = %d\n", m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Capture]);
    MY_LOG("i4SensorHeight[ESensorMode_Capture] = %d\n", m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Capture]);
    MY_LOG("i4SensorWidth[ESensorMode_Video] = %d\n", m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Video]);
    MY_LOG("i4SensorHeight[ESensorMode_Video] = %d\n", m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Video]);
    MY_LOG("i4SensorWidth[ESensorMode_SlimVideo1] = %d\n", m_rAWBWindowConfig.i4SensorWidth[ESensorMode_SlimVideo1]);
    MY_LOG("i4SensorHeight[ESensorMode_SlimVideo1] = %d\n", m_rAWBWindowConfig.i4SensorHeight[ESensorMode_SlimVideo1]);
    MY_LOG("i4SensorWidth[ESensorMode_SlimVideo2] = %d\n", m_rAWBWindowConfig.i4SensorWidth[ESensorMode_SlimVideo2]);
    MY_LOG("i4SensorHeight[ESensorMode_SlimVideo2] = %d\n", m_rAWBWindowConfig.i4SensorHeight[ESensorMode_SlimVideo2]);
    MY_LOG("i4SensorWidth[ESensorMode_Custom1] = %d\n", m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom1]);
    MY_LOG("i4SensorHeight[ESensorMode_Custom1] = %d\n", m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom1]);
    MY_LOG("i4SensorWidth[ESensorMode_Custom2] = %d\n", m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom2]);
    MY_LOG("i4SensorHeight[ESensorMode_Custom2] = %d\n", m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom2]);
    MY_LOG("i4SensorWidth[ESensorMode_Custom3] = %d\n", m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom3]);
    MY_LOG("i4SensorHeight[ESensorMode_Custom3] = %d\n", m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom3]);
    MY_LOG("i4SensorWidth[ESensorMode_Custom4] = %d\n", m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom4]);
    MY_LOG("i4SensorHeight[ESensorMode_Custom4] = %d\n", m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom4]);
    MY_LOG("i4SensorWidth[ESensorMode_Custom5] = %d\n", m_rAWBWindowConfig.i4SensorWidth[ESensorMode_Custom5]);
    MY_LOG("i4SensorHeight[ESensorMode_Custom5] = %d\n", m_rAWBWindowConfig.i4SensorHeight[ESensorMode_Custom5]);


    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::getNvramData()
{
    MY_LOG("[%s()][%d]\n", __FUNCTION__, m_eSensorDev);

    // Get default AWB calibration data
    int err;
    err = NvBufUtil::getInstance().getBufAndRead(CAMERA_NVRAM_DATA_3A, m_eSensorDev, (void*&)m_pNVRAM_3A);
    if(err!=0)
    {
        MY_ERR("getNvramData getBufAndRead fail\n");
        return E_AWB_NVRAM_ERROR;
    }

#if 1
    MY_LOG("sizeof(m_pNVRAM_3A->SensorId) = %d\n", sizeof(m_pNVRAM_3A->SensorId));
    MY_LOG("sizeof(m_pNVRAM_3A->rAENVRAM) = %d\n", sizeof(m_pNVRAM_3A->rAENVRAM));
    MY_LOG("sizeof(m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx]) x 2 = %d\n", sizeof(m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx]));
    MY_LOG("sizeof(m_pNVRAM_3A->rFlashAWBNVRAM) = %d\n", sizeof(m_pNVRAM_3A->rFlashAWBNVRAM));
    MY_LOG("sizeof(m_pNVRAM_3A) = %d\n", sizeof(*m_pNVRAM_3A));
#endif


    /*
    // Get default AWB calibration data
    if (FAILED(NvramDrvMgr::getInstance().init(m_eSensorDev))) {
    MY_ERR("NvramDrvMgr init fail\n");
    return E_AWB_SENSOR_ERROR;
    }

    NvramDrvMgr::getInstance().getRefBuf(m_pNVRAM_3A);
    NvramDrvMgr::getInstance().uninit();

    #if 1
    MY_LOG("sizeof(m_pNVRAM_3A->u4Version) = %d\n", sizeof(m_pNVRAM_3A->u4Version));
    MY_LOG("sizeof(m_pNVRAM_3A->SensorId) = %d\n", sizeof(m_pNVRAM_3A->SensorId));
    MY_LOG("sizeof(m_pNVRAM_3A->rAENVRAM) = %d\n", sizeof(m_pNVRAM_3A->rAENVRAM));
    MY_LOG("sizeof(m_pNVRAM_3A->rAWBNVRAM) = %d\n", sizeof(m_pNVRAM_3A->rAWBNVRAM));
    MY_LOG("sizeof(m_pNVRAM_3A->rFlashAWBNVRAM) = %d\n", sizeof(m_pNVRAM_3A->rFlashAWBNVRAM));
    #endif
    */
    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::getEEPROMData()
{
    MY_LOG("[%s()][%d]\n", __FUNCTION__, m_eSensorDev);

    MUINT32 result=0;
    CAM_CAL_DATA_STRUCT GetCamCalData;
    CamCalDrvBase *pCamCalDrvObj = CamCalDrvBase::createInstance();
    MINT32 i4SensorDevID;

    switch (m_eSensorDev)
    {
    case ESensorDev_Main:
        i4SensorDevID = SENSOR_DEV_MAIN;
        break;
    case ESensorDev_Sub:
        i4SensorDevID = SENSOR_DEV_SUB;
        break;
    case ESensorDev_MainSecond:
        i4SensorDevID = SENSOR_DEV_MAIN_2;
        return S_AWB_OK;
    case ESensorDev_Main3D:
        i4SensorDevID = SENSOR_DEV_MAIN_3D;
        return S_AWB_OK;
    default:
        i4SensorDevID = SENSOR_DEV_NONE;
        return S_AWB_OK;
    }

#if 0
    CAMERA_CAM_CAL_TYPE_ENUM enCamCalEnum=CAMERA_CAM_CAL_DATA_MODULE_VERSION;
    result= m_pCamCalDrvObj->GetCamCalCalData(i4SensorDevID, enCamCalEnum, (void *)&GetCamCalData);
    MY_LOG("(0x%8x)=m_pCamCalDrvObj->GetCamCalCalData", result);
    if(result&CamCalReturnErr[enCamCalEnum])
    {
        MY_LOG("err (%s)", CamCalErrString[enCamCalEnum]);
    }
    else
    {
        MY_LOG("NO err (%s)", CamCalErrString[enCamCalEnum]);
    }
    enCamCalEnum = CAMERA_CAM_CAL_DATA_PART_NUMBER;
    result= m_pCamCalDrvObj->GetCamCalCalData(i4SensorDevID, enCamCalEnum, (void *)&GetCamCalData);
    MY_LOG("(0x%8x)=m_pCamCalDrvObj->GetCamCalCalData", result);
    if(result&CamCalReturnErr[enCamCalEnum])
    {
        MY_LOG("err (%s)", CamCalErrString[enCamCalEnum]);
    }
    else
    {
        MY_LOG("NO err (%s)", CamCalErrString[enCamCalEnum]);
    }
#endif

    CAMERA_CAM_CAL_TYPE_ENUM enCamCalEnum = CAMERA_CAM_CAL_DATA_3A_GAIN;
    result= pCamCalDrvObj->GetCamCalCalData(i4SensorDevID, enCamCalEnum, (void *)&GetCamCalData);
    MY_LOG("(0x%8x)=pCamCalDrvObj->GetCamCalCalData", result);

    if (result&CamCalReturnErr[enCamCalEnum])
    {
        MY_LOG("err (%s)", CamCalErrString[enCamCalEnum]);
    }
    else
    {
        MY_LOG("NO err (%s)", CamCalErrString[enCamCalEnum]);
        m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rGoldenGain.i4R = static_cast<MINT32>(GetCamCalData.Single2A.S2aAwb.rGoldGainu4R);
        m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rGoldenGain.i4G = static_cast<MINT32>(GetCamCalData.Single2A.S2aAwb.rGoldGainu4G);
        m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rGoldenGain.i4B = static_cast<MINT32>(GetCamCalData.Single2A.S2aAwb.rGoldGainu4B);
        m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rUnitGain.i4R = static_cast<MINT32>(GetCamCalData.Single2A.S2aAwb.rUnitGainu4R);
        m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rUnitGain.i4G = static_cast<MINT32>(GetCamCalData.Single2A.S2aAwb.rUnitGainu4G);
        m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rUnitGain.i4B = static_cast<MINT32>(GetCamCalData.Single2A.S2aAwb.rUnitGainu4B);
    }

    MY_LOG("m_pNVRAM_3A->rAWBNVRAM[%d].rCalData.rGoldenGain.i4R = %d\n", m_i4AWBNvramIdx, m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rGoldenGain.i4R);
    MY_LOG("m_pNVRAM_3A->rAWBNVRAM[%d].rCalData.rGoldenGain.i4G = %d\n", m_i4AWBNvramIdx, m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rGoldenGain.i4G);
    MY_LOG("m_pNVRAM_3A->rAWBNVRAM[%d].rCalData.rGoldenGain.i4B = %d\n", m_i4AWBNvramIdx, m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rGoldenGain.i4B);
    MY_LOG("m_pNVRAM_3A->rAWBNVRAM[%d].rCalData.rUnitGain.i4R = %d\n", m_i4AWBNvramIdx, m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rUnitGain.i4R);
    MY_LOG("m_pNVRAM_3A->rAWBNVRAM[%d].rCalData.rUnitGain.i4G = %d\n", m_i4AWBNvramIdx, m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rUnitGain.i4G);
    MY_LOG("m_pNVRAM_3A->rAWBNVRAM[%d].rCalData.rUnitGain.i4B = %d\n", m_i4AWBNvramIdx, m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rUnitGain.i4B);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::queryHBinInfo()
{
#if 0 //Fix ME
    INormalPipe* pPipe = INormalPipe::createInstance(m_i4SensorIdx,"awb_mgr");//iopipe2.0
    NormalPipe_HBIN_Info rNormalPipe_HBIN_Info;

    // query HBIN info
    if (MFALSE == pPipe->sendCommand(NSImageio::NSIspio::EPIPECmd_GET_HBIN_INFO, m_i4SensorIdx, (MINTPTR)&rNormalPipe_HBIN_Info, MNULL))
    {
        // Error Handling
        MY_ERR("EPIPECmd_GET_HBIN_INFO fail");
        pPipe->destroyInstance("awb_mgr");
        return MFALSE;
    }

    m_bHBIN2Enable = rNormalPipe_HBIN_Info.mEnabled;

    MY_LOG("m_bHBIN2Enable = %d\n", m_bHBIN2Enable);

    pPipe->destroyInstance("awb_mgr");
#endif

    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::AWBWindowConfig()
{
    for (MINT32 sensorMode = ESensorMode_Preview; sensorMode < ESensorMode_NUM; sensorMode++)
    {
        // Window number: 1x1 up to 128x128
        m_rAWBWindowConfig.i4NumX[sensorMode] = m_rAWBStatParam.i4WindowNumX;
        m_rAWBWindowConfig.i4NumY[sensorMode] = m_rAWBStatParam.i4WindowNumY;

        // Window pitch
        m_rAWBWindowConfig.i4PitchX[sensorMode] = m_rAWBWindowConfig.i4SensorWidth[sensorMode] / m_rAWBWindowConfig.i4NumX[sensorMode];
        m_rAWBWindowConfig.i4PitchY[sensorMode] = m_rAWBWindowConfig.i4SensorHeight[sensorMode] / m_rAWBWindowConfig.i4NumY[sensorMode];

        // Window size: minimum horizontal size = 4, minimum vertical size = 2
        m_rAWBWindowConfig.i4SizeX[sensorMode] = (m_rAWBWindowConfig.i4PitchX[sensorMode] / 2) * 2; // even number
        m_rAWBWindowConfig.i4SizeY[sensorMode] = (m_rAWBWindowConfig.i4PitchY[sensorMode] / 2) * 2; // even number

        if (m_rAWBWindowConfig.i4SizeX[sensorMode] < 4) {
            MY_LOG("[%d][AWBWindowConfig] i4SizeX < 4: %d\n", m_eSensorDev, m_rAWBWindowConfig.i4SizeX[sensorMode]);
            m_rAWBWindowConfig.i4SizeX[sensorMode] = 4;
        }

        if (m_rAWBWindowConfig.i4SizeY[sensorMode] < 2) {
            MY_LOG("[%d][AWBWindowConfig] i4SizeY < 2: %d\n", m_eSensorDev, m_rAWBWindowConfig.i4SizeY[sensorMode]);
            m_rAWBWindowConfig.i4SizeY[sensorMode] = 2;
        }

        // Window origin
        m_rAWBWindowConfig.i4OriginX[sensorMode] = (m_rAWBWindowConfig.i4SensorWidth[sensorMode] - (m_rAWBWindowConfig.i4PitchX[sensorMode] * m_rAWBWindowConfig.i4NumX[sensorMode])) / 2;
        m_rAWBWindowConfig.i4OriginY[sensorMode] = (m_rAWBWindowConfig.i4SensorHeight[sensorMode] - (m_rAWBWindowConfig.i4PitchY[sensorMode] * m_rAWBWindowConfig.i4NumY[sensorMode])) / 2;

        MY_LOG("[%d][AWBWindowConfig] Sensor mode: %d\n", m_eSensorDev, sensorMode);
        MY_LOG("[%d][AWBWindowConfig] i4SensorWidth: %d, i4SensorHeight: %d\n", m_eSensorDev, m_rAWBWindowConfig.i4SensorWidth[sensorMode], m_rAWBWindowConfig.i4SensorHeight[sensorMode]);
        MY_LOG("[%d][AWBWindowConfig] i4NumX: %d, i4NumY: %d\n", m_eSensorDev, m_rAWBWindowConfig.i4NumX[sensorMode], m_rAWBWindowConfig.i4NumY[sensorMode]);
        MY_LOG("[%d][AWBWindowConfig] i4SizeX: %d, i4SizeY: %d\n", m_eSensorDev, m_rAWBWindowConfig.i4SizeX[sensorMode], m_rAWBWindowConfig.i4SizeY[sensorMode]);
        MY_LOG("[%d][AWBWindowConfig] i4PitchX: %d, i4PitchY: %d\n", m_eSensorDev, m_rAWBWindowConfig.i4PitchX[sensorMode], m_rAWBWindowConfig.i4PitchY[sensorMode]);
        MY_LOG("[%d][AWBWindowConfig] i4OriginX: %d, i4OriginY: %d\n", m_eSensorDev, m_rAWBWindowConfig.i4OriginX[sensorMode], m_rAWBWindowConfig.i4OriginY[sensorMode]);
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::AWBStatConfig()
{
    MINT32 sensorMode, awbMode;

    // STROBE ON
    for (sensorMode = ESensorMode_Preview; sensorMode < ESensorMode_NUM; sensorMode++)
    {
        MINT32 i4WindowPixelNumR = (m_rAWBWindowConfig.i4SizeX[sensorMode] * m_rAWBWindowConfig.i4SizeY[sensorMode]) / 4;
        MINT32 i4WindowPixelNumG = i4WindowPixelNumR * 2;
        MINT32 i4WindowPixelNumB = i4WindowPixelNumR;
        MINT32 i4PixelCountR = ((1 << 24) + (i4WindowPixelNumR >> 1)) / i4WindowPixelNumR;
        MINT32 i4PixelCountG = ((1 << 24) + (i4WindowPixelNumG >> 1)) / i4WindowPixelNumG;
        MINT32 i4PixelCountB = ((1 << 24) + (i4WindowPixelNumB >> 1)) / i4WindowPixelNumB;

        // 1. Update AWB window config, RAW pregain1, and rotation matrix
        for (awbMode = LIB3A_AWB_MODE_OFF; awbMode < LIB3A_AWB_MODE_NUM; awbMode++)
        {
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4WindowSizeX = m_rAWBWindowConfig.i4SizeX[sensorMode];
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4WindowSizeY = m_rAWBWindowConfig.i4SizeY[sensorMode];

            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4WindowPitchX = m_rAWBWindowConfig.i4PitchX[sensorMode];
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4WindowPitchY = m_rAWBWindowConfig.i4PitchY[sensorMode];

            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4WindowOriginX = m_rAWBWindowConfig.i4OriginX[sensorMode];
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4WindowOriginY = m_rAWBWindowConfig.i4OriginY[sensorMode];

            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4WindowNumX = m_rAWBWindowConfig.i4NumX[sensorMode];
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4WindowNumY = m_rAWBWindowConfig.i4NumY[sensorMode];

            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4LowThresholdR = m_rAWBStatParam.i4LowThresholdR;
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4LowThresholdG = m_rAWBStatParam.i4LowThresholdG;
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4LowThresholdB = m_rAWBStatParam.i4LowThresholdB;

            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4HighThresholdR = m_rAWBStatParam.i4HighThresholdR;
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4HighThresholdG = m_rAWBStatParam.i4HighThresholdG;
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4HighThresholdB = m_rAWBStatParam.i4HighThresholdB;

            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4PixelCountR = i4PixelCountR;
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4PixelCountG = i4PixelCountG;
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4PixelCountB = i4PixelCountB;

            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4PreGainLimitR = m_rAWBStatParam.i4PreGainLimitR;
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4PreGainLimitG = m_rAWBStatParam.i4PreGainLimitG;
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4PreGainLimitB = m_rAWBStatParam.i4PreGainLimitB;

            //m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4PreGainR = m_rAWBRAWPreGain1.i4R;
            //m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4PreGainG = m_rAWBRAWPreGain1.i4G;
            //m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4PreGainB = m_rAWBRAWPreGain1.i4B;

            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4ErrorThreshold = m_rAWBStatParam.i4ErrorThreshold;

            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4Cos = m_rAWBInitInput.rAWBNVRAM.rAlgoCalParam.rRotationMatrix.i4Cos;
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode].i4Sin = m_rAWBInitInput.rAWBNVRAM.rAlgoCalParam.rRotationMatrix.i4Sin;
        }


        MY_LOG("[AWBStatConfig] sensorMode = %d\n", sensorMode);
        MY_LOG("[AWBStatConfig] i4WindowNumX = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4WindowNumX);
        MY_LOG("[AWBStatConfig] i4WindowNumY = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4WindowNumY);
        MY_LOG("[AWBStatConfig] i4WindowPitchX = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4WindowPitchX);
        MY_LOG("[AWBStatConfig] i4WindowPitchY = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4WindowPitchY);
        MY_LOG("[AWBStatConfig] i4WindowSizeX = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4WindowSizeX);
        MY_LOG("[AWBStatConfig] i4WindowSizeY = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4WindowSizeY);
        MY_LOG("[AWBStatConfig] i4WindowOriginX = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4WindowOriginX);
        MY_LOG("[AWBStatConfig] i4WindowOriginY = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4WindowOriginY);
        MY_LOG("[AWBStatConfig] i4WindowPixelNumR = %d\n", i4WindowPixelNumR);
        MY_LOG("[AWBStatConfig] i4WindowPixelNumG = %d\n", i4WindowPixelNumG);
        MY_LOG("[AWBStatConfig] i4WindowPixelNumB = %d\n", i4WindowPixelNumB);
        MY_LOG("[AWBStatConfig] i4PixelCountR = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4PixelCountR);
        MY_LOG("[AWBStatConfig] i4PixelCountG = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4PixelCountG);
        MY_LOG("[AWBStatConfig] i4PixelCountB = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4PixelCountB);
        MY_LOG("[AWBStatConfig] i4Cos = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4Cos);
        MY_LOG("[AWBStatConfig] i4Sin = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4Sin);

        // 2. Update AWB light area
        AWB_LIGHT_AREA_T *pAWBArea = &m_rAWBInitInput.rAWBNVRAM.rAlgoCalParam.rAWBLightArea;

        // Tungsten
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINR[AWB_LIGHT_TUNGSTEN] = pAWBArea->rTungsten.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINL[AWB_LIGHT_TUNGSTEN] = pAWBArea->rTungsten.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WIND[AWB_LIGHT_TUNGSTEN] = pAWBArea->rTungsten.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINU[AWB_LIGHT_TUNGSTEN] = pAWBArea->rTungsten.i4UpperBound;

        // Warm fluorescent
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINR[AWB_LIGHT_WARM_FLUORESCENT] = pAWBArea->rWarmFluorescent.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINL[AWB_LIGHT_WARM_FLUORESCENT] = pAWBArea->rWarmFluorescent.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WIND[AWB_LIGHT_WARM_FLUORESCENT] = pAWBArea->rWarmFluorescent.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINU[AWB_LIGHT_WARM_FLUORESCENT] = pAWBArea->rWarmFluorescent.i4UpperBound;

        // Fluorescent
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINR[AWB_LIGHT_FLUORESCENT] = pAWBArea->rFluorescent.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINL[AWB_LIGHT_FLUORESCENT] = pAWBArea->rFluorescent.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WIND[AWB_LIGHT_FLUORESCENT] = pAWBArea->rFluorescent.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINU[AWB_LIGHT_FLUORESCENT] = pAWBArea->rFluorescent.i4UpperBound;

        // CWF
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINR[AWB_LIGHT_CWF] = pAWBArea->rCWF.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINL[AWB_LIGHT_CWF] = pAWBArea->rCWF.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WIND[AWB_LIGHT_CWF] = pAWBArea->rCWF.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINU[AWB_LIGHT_CWF] = pAWBArea->rCWF.i4UpperBound;

        // Daylight
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINR[AWB_LIGHT_DAYLIGHT] = pAWBArea->rDaylight.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINL[AWB_LIGHT_DAYLIGHT] = pAWBArea->rDaylight.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WIND[AWB_LIGHT_DAYLIGHT] = pAWBArea->rDaylight.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINU[AWB_LIGHT_DAYLIGHT] = pAWBArea->rDaylight.i4UpperBound;

        // Shade
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINR[AWB_LIGHT_SHADE] = pAWBArea->rShade.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINL[AWB_LIGHT_SHADE] = pAWBArea->rShade.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WIND[AWB_LIGHT_SHADE] = pAWBArea->rShade.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINU[AWB_LIGHT_SHADE] = pAWBArea->rShade.i4UpperBound;

        // Daylight fluorescent
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINR[AWB_LIGHT_DAYLIGHT_FLUORESCENT] = pAWBArea->rDaylightFluorescent.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINL[AWB_LIGHT_DAYLIGHT_FLUORESCENT] = pAWBArea->rDaylightFluorescent.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WIND[AWB_LIGHT_DAYLIGHT_FLUORESCENT] = pAWBArea->rDaylightFluorescent.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINU[AWB_LIGHT_DAYLIGHT_FLUORESCENT] = pAWBArea->rDaylightFluorescent.i4UpperBound;

        // Strobe
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINR[AWB_LIGHT_STROBE] = pAWBArea->rStrobe.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINL[AWB_LIGHT_STROBE] = pAWBArea->rStrobe.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WIND[AWB_LIGHT_STROBE] = pAWBArea->rStrobe.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINU[AWB_LIGHT_STROBE] = pAWBArea->rStrobe.i4UpperBound;

        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rTungsten.i4RightBound = %d\n", m_eSensorDev, pAWBArea->rTungsten.i4RightBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rTungsten.i4LeftBound = %d\n", m_eSensorDev, pAWBArea->rTungsten.i4LeftBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rTungsten.i4LowerBound = %d\n", m_eSensorDev, pAWBArea->rTungsten.i4LowerBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rTungsten.i4UpperBound = %d\n", m_eSensorDev, pAWBArea->rTungsten.i4UpperBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rWarmFluorescent.i4RightBound = %d\n", m_eSensorDev, pAWBArea->rWarmFluorescent.i4RightBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rWarmFluorescent.i4LeftBound = %d\n", m_eSensorDev, pAWBArea->rWarmFluorescent.i4LeftBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rWarmFluorescent.i4LowerBound = %d\n", m_eSensorDev, pAWBArea->rWarmFluorescent.i4LowerBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rWarmFluorescent.i4UpperBound = %d\n", m_eSensorDev, pAWBArea->rWarmFluorescent.i4UpperBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rFluorescent.i4RightBound = %d\n", m_eSensorDev, pAWBArea->rFluorescent.i4RightBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rFluorescent.i4LeftBound = %d\n", m_eSensorDev, pAWBArea->rFluorescent.i4LeftBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rFluorescent.i4LowerBound = %d\n", m_eSensorDev, pAWBArea->rFluorescent.i4LowerBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rFluorescent.i4UpperBound = %d\n", m_eSensorDev, pAWBArea->rFluorescent.i4UpperBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rCWF.i4RightBound = %d\n", m_eSensorDev, pAWBArea->rCWF.i4RightBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rCWF.i4LeftBound = %d\n", m_eSensorDev, pAWBArea->rCWF.i4LeftBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rCWF.i4LowerBound = %d\n", m_eSensorDev, pAWBArea->rCWF.i4LowerBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rCWF.i4UpperBound = %d\n", m_eSensorDev, pAWBArea->rCWF.i4UpperBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rDaylight.i4RightBound = %d\n", m_eSensorDev, pAWBArea->rDaylight.i4RightBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rDaylight.i4LeftBound = %d\n", m_eSensorDev, pAWBArea->rDaylight.i4LeftBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rDaylight.i4LowerBound = %d\n", m_eSensorDev, pAWBArea->rDaylight.i4LowerBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rDaylight.i4UpperBound = %d\n", m_eSensorDev, pAWBArea->rDaylight.i4UpperBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rDaylightFluorescent.i4RightBound = %d\n", m_eSensorDev, pAWBArea->rDaylightFluorescent.i4RightBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rDaylightFluorescent.i4LeftBound = %d\n", m_eSensorDev, pAWBArea->rDaylightFluorescent.i4LeftBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rDaylightFluorescent.i4LowerBound = %d\n", m_eSensorDev, pAWBArea->rDaylightFluorescent.i4LowerBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rDaylightFluorescent.i4UpperBound = %d\n", m_eSensorDev, pAWBArea->rDaylightFluorescent.i4UpperBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rStrobe.i4RightBound = %d\n", m_eSensorDev, pAWBArea->rStrobe.i4RightBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rStrobe.i4LeftBound = %d\n", m_eSensorDev, pAWBArea->rStrobe.i4LeftBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rStrobe.i4LowerBound = %d\n", m_eSensorDev, pAWBArea->rStrobe.i4LowerBound);
        MY_LOG("[%d][AWBStatConfig] m_rAWBInitInput.rAWBNVRAM.rAWBLightArea.rStrobe.i4UpperBound = %d\n", m_eSensorDev, pAWBArea->rStrobe.i4UpperBound);

        // 3. Update PWB light area
        PWB_LIGHT_AREA_T *pPWBArea = &m_rAWBInitInput.rAWBNVRAM.rAlgoCalParam.rPWBLightArea;

        // Reference area
        for (MINT32 i = LIB3A_AWB_MODE_DAYLIGHT; i < LIB3A_AWB_MODE_NUM; i++)
        {
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][i].i4AWBXY_WINR[PWB_REFERENCE_AREA_INDEX] = pPWBArea->rReferenceArea.i4RightBound;
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][i].i4AWBXY_WINL[PWB_REFERENCE_AREA_INDEX] = pPWBArea->rReferenceArea.i4LeftBound;
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][i].i4AWBXY_WIND[PWB_REFERENCE_AREA_INDEX] = pPWBArea->rReferenceArea.i4LowerBound;
            m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][i].i4AWBXY_WINU[PWB_REFERENCE_AREA_INDEX] = pPWBArea->rReferenceArea.i4UpperBound;
        }

        // Daylight neutral area
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_DAYLIGHT].i4AWBXY_WINR[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rDaylight.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_DAYLIGHT].i4AWBXY_WINL[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rDaylight.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_DAYLIGHT].i4AWBXY_WIND[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rDaylight.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_DAYLIGHT].i4AWBXY_WINU[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rDaylight.i4UpperBound;

        // Cloudy daylight neutral area
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_CLOUDY_DAYLIGHT].i4AWBXY_WINR[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rCloudyDaylight.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_CLOUDY_DAYLIGHT].i4AWBXY_WINL[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rCloudyDaylight.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_CLOUDY_DAYLIGHT].i4AWBXY_WIND[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rCloudyDaylight.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_CLOUDY_DAYLIGHT].i4AWBXY_WINU[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rCloudyDaylight.i4UpperBound;

        // Shade neutral area
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_SHADE].i4AWBXY_WINR[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rShade.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_SHADE].i4AWBXY_WINL[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rShade.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_SHADE].i4AWBXY_WIND[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rShade.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_SHADE].i4AWBXY_WINU[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rShade.i4UpperBound;

        // Twilight neutral area
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_TWILIGHT].i4AWBXY_WINR[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rTwilight.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_TWILIGHT].i4AWBXY_WINL[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rTwilight.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_TWILIGHT].i4AWBXY_WIND[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rTwilight.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_TWILIGHT].i4AWBXY_WINU[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rTwilight.i4UpperBound;

        // Twilight neutral area
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_FLUORESCENT].i4AWBXY_WINR[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rFluorescent.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_FLUORESCENT].i4AWBXY_WINL[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rFluorescent.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_FLUORESCENT].i4AWBXY_WIND[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rFluorescent.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_FLUORESCENT].i4AWBXY_WINU[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rFluorescent.i4UpperBound;

        // Warm fluorescent neutral area
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_WARM_FLUORESCENT].i4AWBXY_WINR[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rWarmFluorescent.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_WARM_FLUORESCENT].i4AWBXY_WINL[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rWarmFluorescent.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_WARM_FLUORESCENT].i4AWBXY_WIND[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rWarmFluorescent.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_WARM_FLUORESCENT].i4AWBXY_WINU[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rWarmFluorescent.i4UpperBound;

        // Incandescent neutral area
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_INCANDESCENT].i4AWBXY_WINR[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rIncandescent.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_INCANDESCENT].i4AWBXY_WINL[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rIncandescent.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_INCANDESCENT].i4AWBXY_WIND[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rIncandescent.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_INCANDESCENT].i4AWBXY_WINU[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rIncandescent.i4UpperBound;

        // Gray world neutral area
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_GRAYWORLD].i4AWBXY_WINR[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rGrayWorld.i4RightBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_GRAYWORLD].i4AWBXY_WINL[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rGrayWorld.i4LeftBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_GRAYWORLD].i4AWBXY_WIND[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rGrayWorld.i4LowerBound;
        m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][LIB3A_AWB_MODE_GRAYWORLD].i4AWBXY_WINU[PWB_NEUTRAL_AREA_INDEX] = pPWBArea->rGrayWorld.i4UpperBound;
    }

    // STROBE OFF
    for (sensorMode = ESensorMode_Preview; sensorMode < ESensorMode_NUM; sensorMode++)
    {
        for (awbMode = LIB3A_AWB_MODE_OFF; awbMode < LIB3A_AWB_MODE_NUM; awbMode++)
        {
            m_rAWBStatCfg[AWB_STROBE_MODE_OFF][sensorMode][awbMode] = m_rAWBStatCfg[AWB_STROBE_MODE_ON][sensorMode][awbMode];
        }
    }

    // Update strobe AWB light area
    for (sensorMode = ESensorMode_Preview; sensorMode < ESensorMode_NUM; sensorMode++)
    {
        m_rAWBStatCfg[AWB_STROBE_MODE_OFF][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINR[AWB_LIGHT_STROBE] = 0;
        m_rAWBStatCfg[AWB_STROBE_MODE_OFF][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINL[AWB_LIGHT_STROBE] = 0;
        m_rAWBStatCfg[AWB_STROBE_MODE_OFF][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WIND[AWB_LIGHT_STROBE] = 0;
        m_rAWBStatCfg[AWB_STROBE_MODE_OFF][sensorMode][LIB3A_AWB_MODE_AUTO].i4AWBXY_WINU[AWB_LIGHT_STROBE] = 0;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::AWBRAWPreGain1Config()
{
    for (MINT32 strobeMode = AWB_STROBE_MODE_ON; strobeMode < AWB_STROBE_MODE_NUM; strobeMode++)
        for (MINT32 sensorMode = ESensorMode_Preview; sensorMode < ESensorMode_NUM; sensorMode++)
            for (MINT32 awbMode = LIB3A_AWB_MODE_OFF; awbMode < LIB3A_AWB_MODE_NUM; awbMode++)
            {
                m_rAWBStatCfg[strobeMode][sensorMode][awbMode].i4PreGainR = m_rAWBRAWPreGain1.i4R;
                m_rAWBStatCfg[strobeMode][sensorMode][awbMode].i4PreGainG = m_rAWBRAWPreGain1.i4G;
                m_rAWBStatCfg[strobeMode][sensorMode][awbMode].i4PreGainB = m_rAWBRAWPreGain1.i4B;
            }

            MY_LOG("[AWBRAWPreGain1Config] i4PreGainR = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][ESensorMode_Preview][LIB3A_AWB_MODE_AUTO].i4PreGainR);
            MY_LOG("[AWBRAWPreGain1Config] i4PreGainG = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][ESensorMode_Preview][LIB3A_AWB_MODE_AUTO].i4PreGainG);
            MY_LOG("[AWBRAWPreGain1Config] i4PreGainB = %d\n", m_rAWBStatCfg[AWB_STROBE_MODE_ON][ESensorMode_Preview][LIB3A_AWB_MODE_AUTO].i4PreGainB);

            return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::setTGInfo(MINT32 const i4TGInfo)
{
    MY_LOG("[%s()] i4TGInfo: %d \n", __FUNCTION__, i4TGInfo);

    switch (i4TGInfo)
    {
    case CAM_TG_1:
        m_eSensorTG = ESensorTG_1;
        break;
    case CAM_TG_2:
        m_eSensorTG = ESensorTG_2;
        break;
    default:
        MY_ERR("i4TGInfo = %d", i4TGInfo);
        return MFALSE;
    }

    return MTRUE;
}

MBOOL AwbMgr::setIsMono(int isMono)
{
	  mIsMono=isMono;
	  return MTRUE;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::setAWBNvramIdx(MINT32 const i4AWBNvramIdx)
{
    if (m_i4AWBNvramIdx != i4AWBNvramIdx) {
        MY_LOG("[%s()] m_i4AWBNvramIdx: %d ==> %d \n", __FUNCTION__, m_i4AWBNvramIdx, i4AWBNvramIdx);
        m_i4AWBNvramIdx = i4AWBNvramIdx;
        m_bAWBNvramIdxChanged = MTRUE;
    }

    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::AWBInit()
{
    MY_LOG("[%s()][%d]\n", __FUNCTION__, m_eSensorDev);

    m_rAWBInitInput.eAWBMode = m_eAWBMode;
    m_bStrobeModeChanged = MFALSE;
    m_bAWBModeChanged = MFALSE;
    m_bOneShotAWB = MTRUE; // do one-shot AWB
    m_bInitState = MTRUE; // init state

    switch (m_eSensorDev)
    {
    case ESensorDev_Main: //  Main Sensor
        m_rAWBInitInput.rAWBParam = getAWBParam<ESensorDev_Main>();
        m_rAWBInitInput.rAWBStatParam = getAWBStatParam<ESensorDev_Main>();
        m_rAWBStatParam = getAWBStatParam<ESensorDev_Main>();
        break;
    case ESensorDev_MainSecond: //  Main Second Sensor
        m_rAWBInitInput.rAWBParam = getAWBParam<ESensorDev_MainSecond>();
        m_rAWBInitInput.rAWBStatParam = getAWBStatParam<ESensorDev_MainSecond>();
        m_rAWBStatParam = getAWBStatParam<ESensorDev_MainSecond>();
        break;
    case ESensorDev_Sub: //  Sub Sensor
        m_rAWBInitInput.rAWBParam = getAWBParam<ESensorDev_Sub>();
        m_rAWBInitInput.rAWBStatParam = getAWBStatParam<ESensorDev_Sub>();
        m_rAWBStatParam = getAWBStatParam<ESensorDev_Sub>();
        break;
    default:
        MY_ERR("m_eSensorDev = %d", m_eSensorDev);
        return MFALSE;
    }

#if 0
    // Query TG info
    IHalSensorList* const pIHalSensorList = IHalSensorList::get();
    IHalSensor* pIHalSensor = pIHalSensorList->createSensor("awb_mgr", m_i4SensorIdx);
    SensorDynamicInfo rSensorDynamicInfo;

    switch  ( m_eSensorDev )
    {
    case ESensorDev_Main:
        pIHalSensor->querySensorDynamicInfo(NSCam::SENSOR_DEV_MAIN, &rSensorDynamicInfo);
        break;
    case ESensorDev_Sub:
        pIHalSensor->querySensorDynamicInfo(NSCam::SENSOR_DEV_SUB, &rSensorDynamicInfo);
        break;
    case ESensorDev_MainSecond:
        pIHalSensor->querySensorDynamicInfo(NSCam::SENSOR_DEV_MAIN_2, &rSensorDynamicInfo);
        break;
    default:    //  Shouldn't happen.
        MY_ERR("Invalid sensor device: %d", m_eSensorDev);
    }

    if  ( pIHalSensor )
        pIHalSensor->destroyInstance("awb_mgr");

    MY_LOG("[%d]TG = %d, pixel mode = %d\n", m_eSensorDev, rSensorDynamicInfo.TgInfo, rSensorDynamicInfo.pixelMode);

    switch (rSensorDynamicInfo.TgInfo)
    {
    case CAM_TG_1:
        m_eSensorTG = ESensorTG_1;
        break;
    case CAM_TG_2:
        m_eSensorTG = ESensorTG_2;
        break;
    default:
        MY_ERR("rSensorDynamicInfo.TgInfo = %d", rSensorDynamicInfo.TgInfo);
        return MFALSE;
    }
#endif

    // HBIN2 enable check
    queryHBinInfo();

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("awb.otp.disable", value, "0");
    MBOOL bDisable = atoi(value);

    property_get("awb.otp.force.read", value, "0");
    MBOOL bForceRead = atoi(value);

    if (m_bAWBCalibrationBypassed) {
        m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rGoldenGain.i4R = 0;
        m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rGoldenGain.i4G = 0;
        m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rGoldenGain.i4B = 0;
        m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rUnitGain.i4R = 0;
        m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rUnitGain.i4G = 0;
        m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rUnitGain.i4B = 0;
    }

    // EEPROM
    if (((!bDisable) &&
        (!m_bAWBCalibrationBypassed) &&
        ((m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rGoldenGain.i4R == 0) ||
        (m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rGoldenGain.i4G == 0) ||
        (m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rGoldenGain.i4B == 0) ||
        (m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rUnitGain.i4R == 0)   ||
        (m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rUnitGain.i4G == 0)   ||
        (m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx].rAlgoCalParam.rCalData.rUnitGain.i4B == 0))) ||
        (bForceRead) ||
        m_bAWBNvramIdxChanged)
    {
        getEEPROMData(); // TBC
    }


    if (m_bColdBoot || bForceRead || m_bAWBNvramIdxChanged) {
        m_rAWBInitInput.rAWBNVRAM = m_pNVRAM_3A->rAWBNVRAM[m_i4AWBNvramIdx];

        MY_LOG("m_rAWBInitInput.eAWBMode = %d\n", m_rAWBInitInput.eAWBMode);
        MY_LOG("m_eSensorDev = %d\n", m_eSensorDev);

        AWBWindowConfig();
        AWBStatConfig();

        m_pIAwbAlgo->setAWBStatConfig(m_rAWBStatCfg[m_i4StrobeMode][m_eSensorMode][m_eAWBMode],
            m_rAWBWindowConfig.i4SensorWidth[m_eSensorMode],
            m_rAWBWindowConfig.i4SensorHeight[m_eSensorMode]);

        if (FAILED(m_pIAwbAlgo->initAWB(m_rAWBInitInput, m_rAWBOutput, m_rAWBRAWPreGain1))) {
            MY_ERR("m_pIAwbAlgo->initAWB fail\n");
            return MFALSE;
        }

        AWBRAWPreGain1Config();

        m_bColdBoot = MFALSE;
        m_bAWBNvramIdxChanged = MFALSE;
    }
    else {
        m_pIAwbAlgo->setAWBMode(m_eAWBMode);
        m_pIAwbAlgo->setAWBStatConfig(m_rAWBStatCfg[m_i4StrobeMode][m_eSensorMode][m_eAWBMode],
            m_rAWBWindowConfig.i4SensorWidth[m_eSensorMode],
            m_rAWBWindowConfig.i4SensorHeight[m_eSensorMode]);
    }

    // reset AWB state
    if (m_eAWBMode == LIB3A_AWB_MODE_AUTO)
        m_pAwbStateMgr->reset(eState_InactiveAuto);
    else
        m_pAwbStateMgr->reset(eState_InactiveNotAuto);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::getColorCorrectionGain(MFLOAT& fGain_R, MFLOAT& fGain_G, MFLOAT& fGain_B)
{
    MY_LOG("%s()\n", __FUNCTION__);

    fGain_R = static_cast<MFLOAT>(m_rAWBOutput.rAWBInfo.rCurrentAWBGain.i4R) / m_rAWBInitInput.rAWBParam.rChipParam.i4AWBGainOutputScaleUnit;
    fGain_G = static_cast<MFLOAT>(m_rAWBOutput.rAWBInfo.rCurrentAWBGain.i4G) / m_rAWBInitInput.rAWBParam.rChipParam.i4AWBGainOutputScaleUnit;
    fGain_B = static_cast<MFLOAT>(m_rAWBOutput.rAWBInfo.rCurrentAWBGain.i4B) / m_rAWBInitInput.rAWBParam.rChipParam.i4AWBGainOutputScaleUnit;

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::setColorCorrectionGain(MFLOAT fGain_R, MFLOAT fGain_G, MFLOAT fGain_B)
{
    MY_LOG("%s() rgain=%5.2f ggain=%5.2f bgain=%5.2f\n", __FUNCTION__, (float)fGain_R, (float)fGain_G, (float)fGain_B);

    m_rColorCorrectionGain.i4R = static_cast<MINT32>((fGain_R * AWB_SCALE_UNIT) + 0.5);
    m_rColorCorrectionGain.i4G = static_cast<MINT32>((fGain_G * AWB_SCALE_UNIT) + 0.5);
    m_rColorCorrectionGain.i4B = static_cast<MINT32>((fGain_B * AWB_SCALE_UNIT) + 0.5);

    if (m_rColorCorrectionGain.i4R > m_rAWBInitInput.rAWBParam.rChipParam.i4AWBGainOutputUpperLimit)
        m_rColorCorrectionGain.i4R = m_rAWBInitInput.rAWBParam.rChipParam.i4AWBGainOutputUpperLimit;

    if (m_rColorCorrectionGain.i4G > m_rAWBInitInput.rAWBParam.rChipParam.i4AWBGainOutputUpperLimit)
        m_rColorCorrectionGain.i4G = m_rAWBInitInput.rAWBParam.rChipParam.i4AWBGainOutputUpperLimit;

    if (m_rColorCorrectionGain.i4B > m_rAWBInitInput.rAWBParam.rChipParam.i4AWBGainOutputUpperLimit)
        m_rColorCorrectionGain.i4B = m_rAWBInitInput.rAWBParam.rChipParam.i4AWBGainOutputUpperLimit;

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::getAWBState(mtk_camera_metadata_enum_android_control_awb_state_t& eAWBState)
{
    MY_LOG("%s()\n", __FUNCTION__);

    eAWBState = m_pAwbStateMgr->getCurrentState();

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AwbMgr::setColorCorrectionMode(MINT32 i4ColorCorrectionMode)
{
    MY_LOG("%s() %d\n (TRANSFORM_MATRIX=%d MODE_FAST=%d HIGH_QUALITY=%d)", __FUNCTION__, i4ColorCorrectionMode,
        MTK_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX,
        MTK_COLOR_CORRECTION_MODE_FAST,
        MTK_COLOR_CORRECTION_MODE_HIGH_QUALITY
        );

    switch (i4ColorCorrectionMode)
    {
    case MTK_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX:
        m_eColorCorrectionMode = MTK_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX;
        break;
    case MTK_COLOR_CORRECTION_MODE_FAST:
        m_eColorCorrectionMode = MTK_COLOR_CORRECTION_MODE_FAST;
        break;
    case MTK_COLOR_CORRECTION_MODE_HIGH_QUALITY:
        m_eColorCorrectionMode = MTK_COLOR_CORRECTION_MODE_HIGH_QUALITY;
        break;
    default:
        MY_ERR("Incorrect color correction mode = %d", i4ColorCorrectionMode);
    }

    return MTRUE;
}


