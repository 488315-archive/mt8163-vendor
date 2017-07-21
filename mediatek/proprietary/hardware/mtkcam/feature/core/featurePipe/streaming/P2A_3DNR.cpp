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

#include "P2A_3DNR.h"
#include "P2ANode.h"

#include "DebugControl.h"
#define PIPE_CLASS_TAG "P2A_3DNR"
// #define PIPE_TRACE TRACE_P2A_3DNR
#define PIPE_TRACE 0
#include <featurePipe/core/include/PipeLog.h>

#include <iopipe/PostProc/IHalPostProcPipe.h>

using namespace NSCam::NSIoPipe::NSPostProc;
using NSImageio::NSIspio::EPortIndex_IMG3O;
using NSImageio::NSIspio::EPortIndex_VIPI;

// == should be obsolete ===
#include "iopipe/PostProc/IFeatureStream.h"

// debug usage
//#define EFeatureModule_NR3D_USED
//#define ENABLE_3DNR_USING_TUNING_DATA

#define WEAK_GAIN_TEST 1
#define STRONG_GAIN_TEST 1

#define  TUNING_PARAM_TEST_1 // ori. first + James' weak version  -->

// =========================
//
static inline MUINT32 getMagicNo(NSCam::NSIoPipe::NSPostProc::QParams const& rParams)
{
#if 1
    return 1111; //TODO: create 3dnr magicNo
#else
    if (rParams.mvMagicNo.size())
        return rParams.mvMagicNo[0];
    else
        return rParams.mFrameNo;
#endif
}

/// ============
#include <iopipe/PostProc/IHalDpePipe.h>
using namespace NSCam::NSIoPipe::NSDpe;



#if WEAK_GAIN_TEST
void setWeakGain(dip_x_reg_t * pIspPhyReg, MINT32 loglevel)
{
    //    pIspPhyReg->DIP_X_CTL_YUV_EN.Raw=0x01200400;
    if (pIspPhyReg == NULL)
    {
        return;
    }
    MY_LOGD_IF(loglevel >= 2, "mkdbg:: WEAK_GAIN_TEST test started");

    pIspPhyReg->DIP_X_NR3D_FBCNT_OFF.Raw=0x00000000;
    pIspPhyReg->DIP_X_NR3D_FBCNT_SIZ.Raw=0x00000000;
    pIspPhyReg->DIP_X_NR3D_FB_COUNT.Raw=0x00000000;

    pIspPhyReg->DIP_X_NR3D_AY_CON1.Raw=0x04030202; // 1
    pIspPhyReg->DIP_X_NR3D_AY_CON2.Raw=0x07151515;
    pIspPhyReg->DIP_X_NR3D_AY_CON3.Raw=0x00000000; // done
    pIspPhyReg->DIP_X_NR3D_AY_CON4.Raw=0x01000000;
    pIspPhyReg->DIP_X_NR3D_AY_CON5.Raw=0x01c80190;

    pIspPhyReg->DIP_X_NR3D_AC_CON1.Raw=0x04030202; // 6
    pIspPhyReg->DIP_X_NR3D_AC_CON2.Raw=0x08080808;
    pIspPhyReg->DIP_X_NR3D_AC_CON3.Raw=0x00000010;
    pIspPhyReg->DIP_X_NR3D_AC_CON4.Raw=0x01C80000;
    pIspPhyReg->DIP_X_NR3D_AC_CON5.Raw=0x01EC01DC;

    pIspPhyReg->DIP_X_NR3D_B1Y_CON1.Raw=0x1b170d07; // 11
    pIspPhyReg->DIP_X_NR3D_B1Y_CON2.Raw=0x08080808;
    pIspPhyReg->DIP_X_NR3D_B1Y_CON3.Raw=0x00000000;
    pIspPhyReg->DIP_X_NR3D_B1C_CON1.Raw=0x1b170d07;
    pIspPhyReg->DIP_X_NR3D_B1C_CON2.Raw=0x08080808;

    pIspPhyReg->DIP_X_NR3D_B1C_CON3.Raw=0x00000000; // 16
    pIspPhyReg->DIP_X_NR3D_LSCG1_XCON.Raw=0x3f342818;
    pIspPhyReg->DIP_X_NR3D_LSCG1_YCON.Raw=0x2c333f3f;
    pIspPhyReg->DIP_X_NR3D_LSCG1_CON.Raw=0x00880020;
    pIspPhyReg->DIP_X_NR3D_LSCG1_SCON.Raw=0x23131800;

    pIspPhyReg->DIP_X_NR3D_NLY_CON1.Raw=0x002f0002; // 21
    pIspPhyReg->DIP_X_NR3D_NLY_CON2.Raw=0x1f384717;
    pIspPhyReg->DIP_X_NR3D_MLY_CON.Raw=0x00000417;
    pIspPhyReg->DIP_X_NR3D_NLC_CON1.Raw=0x004f0002;
    pIspPhyReg->DIP_X_NR3D_NLC_CON2.Raw=0x17384737;

    pIspPhyReg->DIP_X_NR3D_MLC_CON.Raw=0x0000040e; // 26
    pIspPhyReg->DIP_X_NR3D_SLY_CON.Raw=0x80808000;
    pIspPhyReg->DIP_X_NR3D_SLC_CON.Raw=0x80008000;
    pIspPhyReg->DIP_X_NR3D_DIFG_CON.Raw=0x100c1010;
    pIspPhyReg->DIP_X_NR3D_YCJC_CON1.Raw=0x14282a28;

    pIspPhyReg->DIP_X_NR3D_YCJC_CON2.Raw=0x0011442a; // 31

//    pIspPhyReg->DIP_X_NR3D_SPARE0.Raw=0x8F2DE105;
//    pIspPhyReg->DIP_X_NR3D_SPARE1.Raw=0xB28FC2B1;

}
#endif // WEAK_GAIN_TEST

#if STRONG_GAIN_TEST
void setStrongGain(dip_x_reg_t * pIspPhyReg, MINT32 loglevel)
{
    if (pIspPhyReg == NULL)
    {
        return;
    }
    //    pIspPhyReg->DIP_X_CTL_YUV_EN.Raw=0x01200400;

    MY_LOGD_IF(loglevel >= 2, "mkdbg:: WEAK_GAIN_TEST test started");

    pIspPhyReg->DIP_X_NR3D_FBCNT_OFF.Raw=0x00000000;
    pIspPhyReg->DIP_X_NR3D_FBCNT_SIZ.Raw=0x00000000;
    pIspPhyReg->DIP_X_NR3D_FB_COUNT.Raw=0x00000000;

    pIspPhyReg->DIP_X_NR3D_AY_CON1.Raw=0x1e19140f; // 1
    pIspPhyReg->DIP_X_NR3D_AY_CON2.Raw=0x060f1e1e;
    pIspPhyReg->DIP_X_NR3D_AY_CON3.Raw=0x00000000; // done
    pIspPhyReg->DIP_X_NR3D_AY_CON4.Raw=0x01e80000;
    pIspPhyReg->DIP_X_NR3D_AY_CON5.Raw=0x01f601f2;

    pIspPhyReg->DIP_X_NR3D_AC_CON1.Raw=0x1814100c; // 6
    pIspPhyReg->DIP_X_NR3D_AC_CON2.Raw=0x060f1e1e;
    pIspPhyReg->DIP_X_NR3D_AC_CON3.Raw=0x00000000;
    pIspPhyReg->DIP_X_NR3D_AC_CON4.Raw=0x01e20000;
    pIspPhyReg->DIP_X_NR3D_AC_CON5.Raw=0x01f401ee;

    pIspPhyReg->DIP_X_NR3D_B1Y_CON1.Raw=0x1a160c07; // 11
    pIspPhyReg->DIP_X_NR3D_B1Y_CON2.Raw=0x08080808;
    pIspPhyReg->DIP_X_NR3D_B1Y_CON3.Raw=0x00000000;

    pIspPhyReg->DIP_X_NR3D_B1C_CON1.Raw=0x1a160c07;
    pIspPhyReg->DIP_X_NR3D_B1C_CON2.Raw=0x0b080808;
    pIspPhyReg->DIP_X_NR3D_B1C_CON3.Raw=0x00060000; // 16

    pIspPhyReg->DIP_X_NR3D_LSCG1_XCON.Raw=0x3f34281f8; // 17
    pIspPhyReg->DIP_X_NR3D_LSCG1_YCON.Raw=0x2c333f3f;
    pIspPhyReg->DIP_X_NR3D_LSCG1_CON.Raw=0x00880020;
    pIspPhyReg->DIP_X_NR3D_LSCG1_SCON.Raw=0x23131800;

    pIspPhyReg->DIP_X_NR3D_NLY_CON1.Raw=0x002f0006; // 21
    pIspPhyReg->DIP_X_NR3D_NLY_CON2.Raw=0x1f384717;
    pIspPhyReg->DIP_X_NR3D_MLY_CON.Raw=0x00000417;

    pIspPhyReg->DIP_X_NR3D_NLC_CON1.Raw=0x004f0004;
    pIspPhyReg->DIP_X_NR3D_NLC_CON2.Raw=0x17384737;

    pIspPhyReg->DIP_X_NR3D_MLC_CON.Raw=0x0000040e; // 26

    pIspPhyReg->DIP_X_NR3D_SLY_CON.Raw=0x80808000; // 27
    pIspPhyReg->DIP_X_NR3D_SLC_CON.Raw=0x80008000;

    pIspPhyReg->DIP_X_NR3D_DIFG_CON.Raw=0x100c1010; // 29
    pIspPhyReg->DIP_X_NR3D_YCJC_CON1.Raw=0x14282a28; // 30

    pIspPhyReg->DIP_X_NR3D_YCJC_CON2.Raw=0x0011442a; // 31
    //    pIspPhyReg->DIP_X_NR3D_SPARE0.Raw=0x8F2DE105;
    //    pIspPhyReg->DIP_X_NR3D_SPARE1.Raw=0xB28FC2B1;
}
#endif // STRONG_GAIN_TEST

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

MBOOL P2ANode::do3dnrFlow(
    NSCam::NSIoPipe::NSPostProc::QParams &enqueParams,
    MRect &dst_resizer_rect,
    eis_region &eisInfo,
    MINT32 iso)

{
    TRACE_FUNC_ENTER();

    MY_LOGD_IF(m3dnrLogLevel >= 2, "[P2A_3DNR::do3dnrFlow] "
        "imgiWidth: %d, imgiHeight: %d, "
        "dst_resizer_rect_w: %d, dst_resizer_rect_w: %d, iso: %d",
        (enqueParams.mvIn[0].mBuffer)->getImgSize().w, (enqueParams.mvIn[0].mBuffer)->getImgSize().h,
        dst_resizer_rect.s.w, dst_resizer_rect.s.h,
        iso
    );

    if (m3dnrInitedSuccess != MTRUE)
    {
        MY_LOGE("[P2A_3DNR::do3dnrFlow] mInitedSuccess == FALSE");
        TRACE_FUNC_EXIT();
        return MFALSE;
    }

    MY_LOGD_IF(m3dnrLogLevel >= 2, "mkdbg: iso: %u, imgi: %p, img_vipi: %p",
        iso,
        enqueParams.mvIn[0].mBuffer,
        ((mPrevFullImg != NULL) ? mPrevFullImg->getImageBufferPtr(): NULL)
    );

    MERROR ret = OK;
    NR3D* pNr3dParam;

    char EnableOption[PROPERTY_VALUE_MAX] = {'\0'};
    MBOOL bImg3OOnly = 0;
    MBOOL bDrvNR3DEnabled = 1;
    if (m3dnrDebugLevel != 0)
    {
        property_get("camera.3dnr.img3oonly", EnableOption, "0");
        bImg3OOnly = atoi(EnableOption);
        property_get("camera.3dnr.drv.nr3d.enable", EnableOption, "1");
        bDrvNR3DEnabled = atoi(EnableOption);
    }


    //get ISO from app in metadata
#if 0 // ppp_TODO-2
    MINT32 iso = 0;
    tryGetMetadata<MINT32>(param_p2->inRetApp->getMetadata(), MTK_SENSOR_SENSITIVITY, iso);
#endif

    if (MTRUE != mp3dnr->prepare(getMagicNo(enqueParams), iso))
    {
        MY_LOGW_IF(m3dnrLogLevel >= 2, "3dnr prepare err");
    }
    //get GMV from metadata
    /*
    eis_region eisInfo; // TODO: ppp_03
    IMetadata* pMeta_InHal	= param_p2->inHal->getMetadata();
    queryEisRegion(pMeta_InHal, eisInfo);
    */

    // if (MTRUE != mp3dnr->setGMV(pFrame->getFrameNo(), 0, 0,0,0))
    if (MTRUE != mp3dnr->setGMV(getMagicNo(enqueParams), eisInfo.gmvX, eisInfo.gmvY,
        eisInfo.x_int, eisInfo.y_int))
    {
        MY_LOGW_IF(m3dnrLogLevel >= 2, "3dnr getGMV err");
    }
    if (MTRUE != mp3dnr->checkIMG3OSize(getMagicNo(enqueParams), dst_resizer_rect.s.w, dst_resizer_rect.s.h))
    {
        MY_LOGW_IF(m3dnrLogLevel >= 2, "3dnr checkIMG3OSize err");
    }

    // new:
    ModuleInfo moduleinfo;

    // ppp_TODO-4
    //	if (mvPervious.size() > 0) pPervious = mvPervious.front();
    // mod:
    if (mPrevFullImg == NULL)
    {
        if (MTRUE != mp3dnr->setVipiParams(MFALSE/* vipi is NULL */, 0, 0, 0, 0))
        {
            MY_LOGW_IF(m3dnrLogLevel >= 2, "3dnr configVipi err");
        }
    }
    else
    {
        IImageBuffer *pIMGBufferVIPI = mPrevFullImg->getImageBufferPtr();
        if (MTRUE != mp3dnr->setVipiParams(pIMGBufferVIPI != NULL,
              pIMGBufferVIPI->getImgSize().w, pIMGBufferVIPI->getImgSize().h,
              //pImg3oBuf->getImgFormat(), pIMGBufferVIPI->getBufStridesInBytes(0) // --> replaced by the following line
              pIMGBufferVIPI->getImgFormat(), pIMGBufferVIPI->getBufStridesInBytes(0))
            )
        {
            MY_LOGW_IF(m3dnrLogLevel >= 2, "skip configVipi flow");
        }
        else
        {
            MY_LOGD_IF(m3dnrLogLevel >= 2, "configVipi: address:%p, W/H(%d,%d)", pIMGBufferVIPI,
            pIMGBufferVIPI->getImgSize().w, pIMGBufferVIPI->getImgSize().h);
            /* config Input for VIPI: this part is done in prepareIO(..) */
        }
    }

    if (MTRUE != mp3dnr->get3dnrParams(getMagicNo(enqueParams),
          dst_resizer_rect.s.w, dst_resizer_rect.s.h, pNr3dParam))
    {
        MY_LOGW_IF(m3dnrLogLevel >= 2, "skip config3dnrParams flow");
    }
    else
    {
        //set for nr3d module
#ifdef EFeatureModule_NR3D_USED // old style
        moduleinfo.moduleTag = EFeatureModule_NR3D;
        moduleinfo.moduleStruct   = reinterpret_cast<MVOID*> (pNr3dParam);

        MY_LOGD_IF(m3dnrLogLevel >= 2, "configVipi: config3dnrParams == TRUE");
        enqueParams.mvModuleData.push_back(moduleinfo);
#endif // EFeatureModule_NR3D_USED

//#ifdef ENABLE_3DNR_USING_TUNING_DATA
        if (bImg3OOnly)
        {
            MY_LOGD_IF(m3dnrLogLevel >= 2, "mkdbg:: camera.3dnr.img3oonly=1, NR3D no VIPI");
        }
        else
        {
#if 0 // debug quality usage
            if (enqueParams.mvTuningData.size() >0)
            {
                char gainLevelOption[PROPERTY_VALUE_MAX] = {'\0'};
                if (m3dnrDebugLevel != 0)
                {
                    property_get("camera.3dnr.gain", gainLevelOption, "0");

                    if (gainLevelOption[0] == '0')
                    {
                        setWeakGain(pIspPhyReg, m3dnrLogLevel);
                    }
                    else
                    {
                        setStrongGain(pIspPhyReg, m3dnrLogLevel);
                    }
                }
                else
                {
                    setWeakGain(pIspPhyReg, m3dnrLogLevel);
                }
            }
#endif
        }
//#endif // ENABLE_3DNR_USING_TUNING_DATA
    }
    MY_LOGD_IF(m3dnrLogLevel >= 2, "pNr3dParam: onOff_onOfStX/Y(%d, %d), onSiz_onW/H(%d, %d), vipi_readW/H(%d, %d)",
        pNr3dParam->onOff_onOfStX, pNr3dParam->onOff_onOfStY,
        pNr3dParam->onSiz_onWd, pNr3dParam->onSiz_onHt,
        pNr3dParam->vipi_readW, pNr3dParam->vipi_readH);

    if (MTRUE == mp3dnr->checkStateMachine(NR3D_STATE_WORKING))
    {
        /* config Output: IMG3O: this part is done in prepareIO(..) */
        if (bImg3OOnly)
        {
            MY_LOGD_IF(m3dnrLogLevel >= 2, "mkdbg:: camera.3dnr.img3oonly=1, NR3D no VIPI");
        }
        else
        {
            if (enqueParams.mvTuningData.size() >0)
            {
                dip_x_reg_t *pIspPhyReg = (dip_x_reg_t*) enqueParams.mvTuningData[0];
                if (bDrvNR3DEnabled)
                {
                    pIspPhyReg->DIP_X_CTL_YUV_EN.Raw |= 0x400;
                }
                else
                {
                    pIspPhyReg->DIP_X_CTL_YUV_EN.Raw &= ~0x400;
                    MY_LOGD_IF(m3dnrLogLevel >= 2, "mkdbg:: NR3D disabled by camera.3dnr.drv.nr3d.enable=0 !!");
                }
                pIspPhyReg->DIP_X_NR3D_ON_CON.Raw=0x00100F00;
                pIspPhyReg->DIP_X_NR3D_ON_OFF.Raw=0x00000000;
                pIspPhyReg->DIP_X_NR3D_ON_SIZ.Raw = ((enqueParams.mvIn[0].mBuffer)->getImgSize().h << 16) | ((enqueParams.mvIn[0].mBuffer)->getImgSize().w);
            }
        }
    }
    else
    {
        MY_LOGD_IF(m3dnrLogLevel >= 2, "mkdbg:: MTRUE != mp3dnr->checkStateMachine(NR3D_STATE_WORKING), dont' enable moduletag: NR3D");
    #ifdef EFeatureModule_NR3D_USED
        enqueParams.mvModuleData.pop();
    #endif
        if (enqueParams.mvTuningData.size() >0)
        {
            dip_x_reg_t *pIspPhyReg = (dip_x_reg_t*) enqueParams.mvTuningData[0];
            pIspPhyReg->DIP_X_CTL_YUV_EN.Raw &= ~0x400;
        }
    }

    TRACE_FUNC_EXIT();
      return OK;
}


#if 0

/* hardware/mtkcam/drv/include/mt6797/iopipe/PostProc/IHalPostProcPipe.h*/
struct MCrpRsInfo
{
    MINT32    mGroupID;
    MCropRect mCropRect;
    MSize     mResizeDst;
    MCrpRsInfo(): mGroupID(0),
                  mResizeDst(mCropRect.s){}
};
struct  QParams
{
    typedef MVOID                   (*PFN_CALLBACK_T)(QParams& rParams);
    PFN_CALLBACK_T                  mpfnCallback;   //deque call back
    PFN_CALLBACK_T                  mpfnEnQFailCallback;    //call back for enque fail
    PFN_CALLBACK_T                  mpfnEnQBlockCallback;   //call back for enque blocking
    MVOID*  mpCookie;
    android::Vector<MINT32> mvStreamTag;        //v1&v3 usage: cooresponding sw scenario for each frame in Qparams
    android::Vector<MVOID*> mvTuningData;        //v1&v3 usage:  for p2 tuning data
    android::Vector<Input>       mvIn;                           //v1&v3 usage
    android::Vector<Output>    mvOut;                      //v1&v3 usage
    android::Vector<MCrpRsInfo> mvCropRsInfo;       //v1&v3 usaage
    MBOOL                           mDequeSuccess;              //v1&v3 usage
    android::Vector<ModuleInfo>     mvModuleData;   //v1&v3 usage
}
#endif
MVOID P2ANode::dump_Qparam(QParams& rParams, const char *pSep)
{
    if (m3dnrDebugLevel == 0)
        return;

    // start dump process
    char EnableOption[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("camera.3dnr.dump.qparam", EnableOption, "0");
    if (EnableOption[0] == '0')
    {
        return;
    }
    if (m3dnrLogLevel == 0)
    {
        return;
    }

    TRACE_FUNC_ENTER();
    MUINT32 i =0;

    MY_LOGD_IF(m3dnrLogLevel >= 2, "%s_: rParams.mDequeSuccess: %d", pSep, rParams.mDequeSuccess);
    if (pSep != NULL && pSep[0] == 'd' && pSep[1] == 'd' && rParams.mDequeSuccess == 0)
    {
        MY_LOGD_IF(m3dnrLogLevel >= 2, "%s_!!! QPARAM DEQUE  FAIL !!!", pSep);
        return;
    }

    MY_LOGD_IF(m3dnrLogLevel >= 2, "%s_: rParams.mpfnCallback: %p", pSep, rParams.mpfnCallback);
    MY_LOGD_IF(m3dnrLogLevel >= 2, "%s_: rParams.mpCookie: %p", pSep, rParams.mpCookie);
#if 0
    MY_LOGD_IF(m3dnrLogLevel >= 2, "%s_: rParams.mpPrivaData: %p", pSep, rParams.mpPrivaData);
    MY_LOGD_IF(m3dnrLogLevel >= 2, "%s_: rParams.mvPrivaData.size(): %d", pSep, rParams.mvPrivaData.size());
#endif

    MY_LOGD_IF(m3dnrLogLevel >= 2, "%s_: rParams.mvTuningData.size(): %d", pSep, rParams.mvTuningData.size());
    for (i = 0; i < rParams.mvTuningData.size(); ++i)
    {
        dip_x_reg_t *pIspPhyReg = (dip_x_reg_t *) rParams.mvTuningData[i];
        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: pIspPhyReg,DIP_X_CTL_YUV_EN.Raw: 0x%x ", pSep, pIspPhyReg->DIP_X_CTL_YUV_EN.Raw);
    }

    // mvIn
    MY_LOGD_IF(m3dnrLogLevel >= 2, "%s_: rParams.mvIn.size(): %d", pSep, rParams.mvIn.size());
    for (i = 0; i < rParams.mvIn.size(); ++i)
    {
        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: --- rParams.mvIn[#%d]: start --- ", pSep, i);
        Input tmp = rParams.mvIn[i];

        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvIn[%d].portID.index: %d", pSep, i, tmp.mPortID.index);
        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvIn[%d].portID.type: %d", pSep, i, tmp.mPortID.type);
        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvIn[%d].portID.inout: %d", pSep, i, tmp.mPortID.inout);
        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvIn[%d].portID.group: %d", pSep, i, tmp.mPortID.group);
        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvIn[%d].portID.capbility: %d", pSep, i, tmp.mPortID.capbility);
        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvIn[%d].portID.reserved: %d", pSep, i, tmp.mPortID.reserved);

        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: Input.mBuffer: %p", pSep, tmp.mBuffer);

        if (tmp.mBuffer != NULL)
        {
            MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvIn[%d].mBuffer.getImgFormat(): %d", pSep, i, tmp.mBuffer->getImgFormat());
            MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvIn[%d].mBuffer.getImgSize(): w=%d, h=%d", pSep, i, tmp.mBuffer->getImgSize().w, tmp.mBuffer->getImgSize().h);
            MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvIn[%d].mBuffer.getImgBitsPerPixel(): %d", pSep, i, tmp.mBuffer->getImgBitsPerPixel());

            MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvIn[%d].mBuffer.getPlaneCount(): %d", pSep, i, tmp.mBuffer->getPlaneCount());
            for (int k =0; k < tmp.mBuffer->getPlaneCount(); ++k)
            {
                MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvIn[%d].mBuffer.getPlaneBitsPerPixel(%d): %d", pSep, i, k, tmp.mBuffer->getPlaneBitsPerPixel(k));
            }
            MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvIn[%d].mBuffer.getBitstreamSize(): %d", pSep, i, tmp.mBuffer->getBitstreamSize());
            //            virtual IImageBufferHeap*       getImageBufferHeap()                const   = 0;
            //            MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvIn[%d].mBuffer.getExtOffsetInBytes(): %d", pSep, i, tmp.mBuffer->getExtOffsetInBytes());
            //!!NOTES: if VA/PA is going to be used, remember to use lockBuf()/unlockBuf()
            //            (tmp.mBuffer)->lockBuf(PIPE_CLASS_TAG, eBUFFER_USAGE_SW_READ_OFTEN);
            //            (tmp.mBuffer)->unlockBuf(PIPE_CLASS_TAG);
        }

        MY_LOGD_IF(m3dnrLogLevel >= 2, "%s_: --- rParams.mvIn[#%d]: end --- ", pSep, i);
    }

    // mvOut
    MY_LOGD_IF(m3dnrLogLevel >= 2, "%s_: rParams.mvOut.size(): %d", pSep, rParams.mvOut.size());
    for (i = 0; i < rParams.mvOut.size(); ++i)
    {
        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: --- rParams.mvOut[#%d]: start --- ", pSep, i);
        Output tmp = rParams.mvOut[i];

        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvOut[%d].portID.index: %d", pSep, i, tmp.mPortID.index);
        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvOut[%d].portID.type: %d", pSep, i, tmp.mPortID.type);
        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvOut[%d].portID.inout: %d", pSep, i, tmp.mPortID.inout);
        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvOut[%d].portID.group: %d", pSep, i, tmp.mPortID.group);
        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvOut[%d].portID.capbility: %d", pSep, i, tmp.mPortID.capbility);
        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvOut[%d].portID.reserved: %d", pSep, i, tmp.mPortID.reserved);

        MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: Input.mBuffer: %p", pSep, tmp.mBuffer);

        if (tmp.mBuffer != NULL)
        {
            MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvOut[%d].mBuffer.getImgFormat(): %d", pSep, i, tmp.mBuffer->getImgFormat());
            MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvOut[%d].mBuffer.getImgSize(): w=%d, h=%d", pSep, i, tmp.mBuffer->getImgSize().w, tmp.mBuffer->getImgSize().h);
            MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvOut[%d].mBuffer.getImgBitsPerPixel(): %d", pSep, i, tmp.mBuffer->getImgBitsPerPixel());

            MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvOut[%d].mBuffer.getPlaneCount(): %d", pSep, i, tmp.mBuffer->getPlaneCount());
            for (int k =0; k < tmp.mBuffer->getPlaneCount(); ++k)
            {
                MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvOut[%d].mBuffer.getPlaneBitsPerPixel(%d): %d", pSep, i, k, tmp.mBuffer->getPlaneBitsPerPixel(k));
            }
            MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvOut[%d].mBuffer.getBitstreamSize(): %d", pSep, i, tmp.mBuffer->getBitstreamSize());
            //            virtual IImageBufferHeap*       getImageBufferHeap()                const   = 0;
            //            MY_LOGD_IF(m3dnrLogLevel >= 2, "\t%s_: mvOut[%d].mBuffer.getExtOffsetInBytes(): %d", pSep, i, tmp.mBuffer->getExtOffsetInBytes());
            //!!NOTES: if VA/PA is going to be used, remember to use lockBuf()/unlockBuf()
            //            (tmp.mBuffer)->lockBuf(PIPE_CLASS_TAG, eBUFFER_USAGE_SW_READ_OFTEN);
            //            (tmp.mBuffer)->unlockBuf(PIPE_CLASS_TAG);
        }

        MY_LOGD_IF(m3dnrLogLevel >= 2, "%s_: --- rParams.mvOut[#%d]: end --- ", pSep, i);
    }

    TRACE_FUNC_EXIT();
}

MVOID P2ANode::dump_vOutImageBuffer(const QParams & params)
{
    // === default values initialized ===
    static int num_img3o_frame_to_dump = 0;
    static int dumped_frame_count = 0;
    static int is_dump_complete = 1;
    static int dump_round_count = 1;
    // ==================================

    // start dump process
    char EnableOption[PROPERTY_VALUE_MAX] = {'\0'};
    if (num_img3o_frame_to_dump == 0  || dumped_frame_count == num_img3o_frame_to_dump)
    {

        num_img3o_frame_to_dump = 0;
        if (m3dnrDebugLevel != 0)
        {
            property_get("camera.3dnr.dump.img3o", EnableOption, "0");
            num_img3o_frame_to_dump = atoi(EnableOption);
        }
//        MY_LOGW("(dumped_frame_count=%d, num_img3o_frame_to_dump =%d), no need to dump vOut frames",
  //          dumped_frame_count, num_img3o_frame_to_dump);
        return;
    }

    //debug: start
    char vOut0_frame_str[64];
    char vOutIMG3O_frame_str[64];

    // start from scratch
    if (is_dump_complete)
    {
        is_dump_complete = 0;
        dumped_frame_count = 0;
    }

    if (dumped_frame_count  < num_img3o_frame_to_dump )
    {
        int is_img3o_dumped = 0;
        MY_LOGD_IF(m3dnrLogLevel >= 2, "mvOut size = %d", params.mvOut.size());

        for (int i =0; i < params.mvOut.size(); ++i)
        {

//            MY_LOGD_IF(m3dnrLogLevel >= 2, "mkdbg: mvOut[%d].mPortID: %d", i, params.mvOut[i].mPortID);
            if (i == 0)
            {
                if (params.mDequeSuccess != 0)
                {
                    sprintf(vOut0_frame_str, "/sdcard/vOut0_frame-r%.2d_%.3d_%dx%d_OK.yuv",
                        dump_round_count, dumped_frame_count,
                        params.mvOut[0].mBuffer->getImgSize().w, params.mvOut[0].mBuffer->getImgSize().h
                        );
                }
                else
                {
                    sprintf(vOut0_frame_str, "/sdcard/vOut0_frame-r%.2d_%.3d_%dx%d_NG.yuv",
                        dump_round_count, dumped_frame_count,
                        params.mvOut[0].mBuffer->getImgSize().w, params.mvOut[0].mBuffer->getImgSize().h
                        );
                }
                params.mvOut[0].mBuffer->saveToFile(vOut0_frame_str);
                MY_LOGD_IF(m3dnrLogLevel >= 2, "params.mvOut[0] saved: %p", params.mvOut[0].mBuffer);
            }

            if (params.mvOut[i].mPortID == PortID(EPortType_Memory, EPortIndex_IMG3O, PORTID_OUT) )
            {
                if (params.mDequeSuccess != 0)
                {
                    sprintf(vOutIMG3O_frame_str, "/sdcard/vOutIMG3O_frame-r%.2d_%.3d_%dx%d_OK.yuv",
                        dump_round_count, dumped_frame_count,
                        params.mvOut[i].mBuffer->getImgSize().w, params.mvOut[i].mBuffer->getImgSize().h
                        );
                }
                else
                {
                    sprintf(vOutIMG3O_frame_str, "/sdcard/vOutIMG3O_frame-r%.2d_%.3d_%dx%d_NG.yuv",
                        dump_round_count, dumped_frame_count,
                        params.mvOut[i].mBuffer->getImgSize().w, params.mvOut[i].mBuffer->getImgSize().h
                        );
                }
                params.mvOut[i].mBuffer->saveToFile(vOutIMG3O_frame_str);
                MY_LOGD_IF(m3dnrLogLevel >= 2, "params.mvOut[%d] EPortIndex_IMG3O saved: %p", i, params.mvOut[i].mBuffer);
                is_img3o_dumped = 1;
            }
        }

        if (is_img3o_dumped == 0)
        {
            MY_LOGD_IF(m3dnrLogLevel >= 2, "mkdbg: !!err: no IMG3O buffer dumped");
            MY_LOGD_IF(m3dnrLogLevel >= 2, "mkdbg: !!err: no IMG3O buffer dumped");
        }
        ++dumped_frame_count;

        if (dumped_frame_count  >= num_img3o_frame_to_dump)
        {
            // when the dump is complete...
            is_dump_complete = 1;
            num_img3o_frame_to_dump = 0;
            MY_LOGD_IF(m3dnrLogLevel >= 2, "dump round %.2d finished ... (dumped_frame_count=%d, num_img3o_frame_to_dump =%d)",
                dump_round_count++, dumped_frame_count, num_img3o_frame_to_dump);
        }
    }

}


MVOID P2ANode::dump_imgiImageBuffer(const QParams & params)
{
    if (m3dnrDebugLevel == 0)
        return;
#if 1
#warning [TODO] dump_imgiImageBuffer

  #if 0
    // start dump process
    char EnableOption[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("camera.3dnr.dump.imgi", EnableOption, "0");
    if (EnableOption[0] == '1')
    {
        MY_LOGW_IF(m3dnrLogLevel >= 2, "(need to dump_imgiImageBuffer(..) ");
    }
    else
    {
        MY_LOGW_IF(m3dnrLogLevel >= 2, "(NO need to dump_imgiImageBuffer(..) ");
    }
  #endif
    return;

#else
    //debug: start
    char vIn0_frame_str[64];
    static int incoming_frame_count = 0;
    static int saved_frame_count = 0;

    if (saved_frame_count <= 5) // dump the first three frames
    //  if (is_frame_saved == 0) // dump the first one only
    {
        sprintf(vIn0_frame_str, "/sdcard/vIn0_frame-%.3d_%dx%d.raw",
            incoming_frame_count,
            params.mvIn[0].mBuffer->getImgSize().w, params.mvIn[0].mBuffer->getImgSize().h);

        params.mvIn[0].mBuffer->saveToFile(vIn0_frame_str);
        MY_LOGD_IF(m3dnrLogLevel >= 2, "params.mvIn[0] saved: %p", params.mvIn[0].mBuffer);
        ++saved_frame_count;
    }
    incoming_frame_count++;
#endif
}

} // namespace NSFeaturePipe
} // namespace NSCamFeature
} // namespace NSCam
