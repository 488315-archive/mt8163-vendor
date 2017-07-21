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
#define LOG_TAG "tsf_rto2"
#ifndef ENABLE_MY_LOG
#define ENABLE_MY_LOG           (1)
#define GLOBAL_ENABLE_MY_LOG    (1)
#endif

//#define LSC_DBG

#include "TsfRto2.h"
#include <LscUtil.h>

using namespace NSIspTuning;

ILscTsf*
TsfRto2::
createInstance(MUINT32 u4SensorDev)
{
    switch (u4SensorDev)
    {
    default:
    case ESensorDev_Main:
        static TsfRto2 singleton_main(static_cast<MUINT32>(ESensorDev_Main));
        return &singleton_main;
    case ESensorDev_MainSecond:
        static TsfRto2 singleton_main2(static_cast<MUINT32>(ESensorDev_MainSecond));
        return &singleton_main2;
    case ESensorDev_Sub:
        static TsfRto2 singleton_sub(static_cast<MUINT32>(ESensorDev_Sub));
        return &singleton_sub;
    }
}

TsfRto2::
TsfRto2(MUINT32 u4SensorDev)
    : TsfDft(u4SensorDev)
{
    LSC_LOG("Enter Type 2: Ratio Cycle");
}

TsfRto2::
~TsfRto2()
{
    LSC_LOG("Exit Type 2: Ratio Cycle");
}

MBOOL
TsfRto2::
tsfSetRatoTblChg(MUINT32 u4Ratio, const ILscTbl& rBaseTbl)
{
    ILscTbl rRtoTbl(ILscTbl::HWTBL);
    if (rBaseTbl.getRaTbl(u4Ratio, rRtoTbl))
    {
        MTK_TSF_TBL_STRUCT rTsfTbl;
        MTK_TSF_LSC_PARAM_STRUCT rLscCfg;
        ILscTbl::Config rTblCfg = rRtoTbl.getConfig();
        rLscCfg.raw_wd        = rTblCfg.i4ImgWd;
        rLscCfg.raw_ht        = rTblCfg.i4ImgHt;
        rLscCfg.x_offset      = 0;
        rLscCfg.y_offset      = 0;
        rLscCfg.block_wd      = rTblCfg.rCfgBlk.i4BlkW;
        rLscCfg.block_ht      = rTblCfg.rCfgBlk.i4BlkH;
        rLscCfg.x_grid_num    = rTblCfg.i4GridX;
        rLscCfg.y_grid_num    = rTblCfg.i4GridY;
        rLscCfg.block_wd_last = rTblCfg.rCfgBlk.i4BlkLastW;
        rLscCfg.block_ht_last = rTblCfg.rCfgBlk.i4BlkLastH;
        rTsfTbl.pLscConfig = &rLscCfg;
        rTsfTbl.ShadingTbl = (MINT32*)rRtoTbl.getData();
        MRESULT ret = m_prTsf->TsfFeatureCtrl(MTKTSF_FEATURE_SET_TBL_CHANGE, &rTsfTbl, NULL);
        if (ret != S_TSF_OK)
        {
            LSC_ERR("Error(0x%08x): MTKTSF_FEATURE_SET_TBL_CHANGE", ret);
            m_prTsf->TsfReset();
            return MFALSE;
        }
        return MTRUE;
    }
    LSC_ERR("Fail to gen ratio(%d) table", u4Ratio);
    return MFALSE;
}

MBOOL
TsfRto2::
tsfBatch(const TSF_INPUT_STAT_T& rInputStat, const ILscTbl& rInputTbl, ILscTbl& rOutputTbl)
{
    MRESULT ret = S_TSF_OK;
    MUINT32 u4Ratio = rInputStat.u4Ratio;
    MBOOL fgDump = m_u4LogEn & EN_LSC_LOG_TSF_DUMP ? MTRUE : MFALSE;

    LSC_LOG_BEGIN("Sensor(%d), #(%d), ratio(%d)", m_u4SensorDev, rInputStat.u4FrmId, u4Ratio);

    if (!tsfSetRatoTblChg(u4Ratio, m_rLscBaseTbl))
    {
        LSC_ERR("Fail to set table change");
    }

    MTK_TSF_SET_PROC_INFO_STRUCT rProcInfo;
    convert(rInputStat, rInputTbl, rProcInfo);

    m_rTsfResult.ShadingTbl = static_cast<MUINT32*>(rOutputTbl.editData());
    m_rTsfResult.u4TblSize = rOutputTbl.getSize();

    ret = m_prTsf->TsfFeatureCtrl(MTKTSF_FEATURE_BATCH, &rProcInfo, &m_rTsfResult);
    if (ret != S_TSF_OK)
    {
        LSC_ERR("Error(0x%08x): MTKTSF_FEATURE_BATCH", ret);
        m_prTsf->TsfReset();
    }
    else
    {
        tsfSetTbl(rOutputTbl);
        tsfSetSL2(m_rTsfResult.SL2Para);
        if (fgDump)
        {
            char strFile[512] = {'\0'};
            sprintf(strFile, "/sdcard/tsf2/%04d_tsfInStat_bat.bin", rInputStat.u4FrmId);
            rInputStat.dump(strFile);
            sprintf(strFile, "/sdcard/tsf2/%04d_tsfInput_bat.tbl", rInputStat.u4FrmId);
            rInputTbl.dump(strFile);
            sprintf(strFile, "/sdcard/tsf2/%04d_tsfOutput_bat.tbl", rInputStat.u4FrmId);
            rOutputTbl.dump(strFile);
        }
    }

    LSC_LOG_END("Sensor(%d), #(%d)", m_u4SensorDev, rInputStat.u4FrmId);

    return ret == S_TSF_OK;
}

MBOOL
TsfRto2::
tsfBatchCap(const TSF_INPUT_STAT_T& rInputStat, const ILscTbl& rInputTbl, ILscTbl& rOutputTbl)
{
    MRESULT ret = S_TSF_OK;
    MUINT32 u4Ratio = rInputStat.u4Ratio;
    MBOOL fgDump = m_u4LogEn & EN_LSC_LOG_TSF_DUMP ? MTRUE : MFALSE;

    LSC_LOG_BEGIN("Sensor(%d), #(%d), ratio(%d)", m_u4SensorDev, rInputStat.u4FrmId, u4Ratio);

    if (!tsfSetRatoTblChg(u4Ratio, m_rLscBaseTbl))
    {
        LSC_ERR("Fail to set table change");
    }

    MTK_TSF_SET_PROC_INFO_STRUCT rProcInfo;
    convert(rInputStat, rInputTbl, rProcInfo);

    m_rTsfResult.ShadingTbl = static_cast<MUINT32*>(rOutputTbl.editData());
    m_rTsfResult.u4TblSize = rOutputTbl.getSize();

    // disable temporal smooth
    m_prTsf->TsfFeatureCtrl(MTKTSF_FEATURE_CONFIG_SMOOTH, (void*)0, 0);
    // batch
    ret = m_prTsf->TsfFeatureCtrl(MTKTSF_FEATURE_BATCH, &rProcInfo, &m_rTsfResult);
    if (ret != S_TSF_OK)
    {
        LSC_ERR("Error(0x%08x): MTKTSF_FEATURE_BATCH", ret);
        m_prTsf->TsfReset();
    }
    else
    {
        tsfSetTbl(rOutputTbl);
        tsfSetSL2(m_rTsfResult.SL2Para);
        if (fgDump)
        {
            char strFile[512] = {'\0'};
            sprintf(strFile, "/sdcard/tsf2/%04d_tsfInStat_cap.bin", rInputStat.u4FrmId);
            rInputStat.dump(strFile);
            sprintf(strFile, "/sdcard/tsf2/%04d_tsfInput_cap.tbl", rInputStat.u4FrmId);
            rInputTbl.dump(strFile);
            sprintf(strFile, "/sdcard/tsf2/%04d_tsfOutput_cap.tbl", rInputStat.u4FrmId);
            rOutputTbl.dump(strFile);
        }
    }
    // enable temporal smooth
    m_prTsf->TsfFeatureCtrl(MTKTSF_FEATURE_CONFIG_SMOOTH, (void*)1, 0);

    LSC_LOG_END("Sensor(%d), #(%d)", m_u4SensorDev, rInputStat.u4FrmId);

    return ret == S_TSF_OK;
}

MBOOL
TsfRto2::
tsfRun(const TSF_INPUT_STAT_T& rInputStat, const ILscTbl& rInputTbl, const ILscTbl& rTblPrior, ILscTbl& rOutputTbl)
{
    MBOOL fgDump = m_u4LogEn & EN_LSC_LOG_TSF_DUMP ? MTRUE : MFALSE;
    MBOOL fgLogEn = (m_u4LogEn & EN_LSC_LOG_TSF_RUN) ? MTRUE : MFALSE;
    MINT32 i4Case = 0;
    MUINT32 u4Step = m_u4PerFrameStep;
    MUINT32 u4Ratio = rInputStat.u4Ratio;

    MY_LOG_IF(fgLogEn, "[%s +] Sensor(%d), #(%d), ratio(%d), step(%d)", __FUNCTION__, m_u4SensorDev, rInputStat.u4FrmId, u4Ratio, u4Step);

    if (m_u4PerFrameStep == 0)
    {
        // only set proc info at the 1st frame.
        if (!tsfSetProcInfo(rInputStat, rInputTbl))
            return MFALSE;
    }
    m_u4PerFrameStep ++;

    // ratio
    if (m_eState == E_TSF_OPT_DONE)
    {
        if (!tsfSetRatoTblChg(u4Ratio, m_rLscBaseTbl))
        {
            LSC_ERR("Fail to set table change");
        }
    }

    E_TSF_STATE_T eState = tsfMain();
    m_eState = eState;
    if (eState == E_TSF_FAIL)
    {
        i4Case = -1;
    }
    else if (eState == E_TSF_READY)
    {
        if (tsfGetResult(rOutputTbl))
        {
            m_u4PerFrameStep = 0;
            tsfSetTbl(rOutputTbl);
            tsfSetSL2(m_rTsfResult.SL2Para);
            i4Case = 1;
        }
        else
        {
            i4Case = -2;
        }
    }
    else
    {
        i4Case = 0;
        rOutputTbl = rTblPrior;
    }

    if (fgDump)
    {
        char strFile[512] = {'\0'};
        sprintf(strFile, "/sdcard/tsf2/%04d_tsfInStat_%d.bin", rInputStat.u4FrmId, u4Step);
        rInputStat.dump(strFile);
        sprintf(strFile, "/sdcard/tsf2/%04d_tsfInput_%d.tbl", rInputStat.u4FrmId, u4Step);
        rInputTbl.dump(strFile);
        sprintf(strFile, "/sdcard/tsf2/%04d_tsfOutput_%d.tbl", rInputStat.u4FrmId, u4Step);
        rOutputTbl.dump(strFile);
    }

    MY_LOG_IF(fgLogEn, "[%s -] Sensor(%d), #(%d), step(%d), case(%d)", __FUNCTION__, m_u4SensorDev, rInputStat.u4FrmId, u4Step, i4Case);

    return (i4Case >= 0);
}

