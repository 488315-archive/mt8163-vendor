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
 /*
** $Log: ObJectTracking.cpp $
 *
*/

#define LOG_TAG "ObjectTracking"
#include <IObjectTracking.h>
#include "MTKOT.h"
#include "camera_custom_ot.h"
#include "assert.h"
#include <Log.h>

/******************************************************************************
*
*******************************************************************************/
#define OT_LOGV(fmt, arg...)        CAM_LOGV("[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg)
#define OT_LOGD(fmt, arg...)        CAM_LOGD("[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg)
#define OT_LOGI(fmt, arg...)        CAM_LOGI("[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg)
#define OT_LOGW(fmt, arg...)        CAM_LOGW("[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg)
#define OT_LOGE(fmt, arg...)        CAM_LOGE("[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg)
#define OT_LOGA(fmt, arg...)        CAM_LOGA("[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg)
#define OT_LOGF(fmt, arg...)        CAM_LOGF("[%s:%d] " fmt, __FUNCTION__, __LINE__, ##arg)
//
#define OT_LOGV_IF(cond, ...)       do { if ( (cond) ) { OT_LOGV(__VA_ARGS__); } }while(0)
#define OT_LOGD_IF(cond, ...)       do { if ( (cond) ) { OT_LOGD(__VA_ARGS__); } }while(0)
#define OT_LOGI_IF(cond, ...)       do { if ( (cond) ) { OT_LOGI(__VA_ARGS__); } }while(0)
#define OT_LOGW_IF(cond, ...)       do { if ( (cond) ) { OT_LOGW(__VA_ARGS__); } }while(0)
#define OT_LOGE_IF(cond, ...)       do { if ( (cond) ) { OT_LOGE(__VA_ARGS__); } }while(0)
#define OT_LOGA_IF(cond, ...)       do { if ( (cond) ) { OT_LOGA(__VA_ARGS__); } }while(0)
#define OT_LOGF_IF(cond, ...)       do { if ( (cond) ) { OT_LOGF(__VA_ARGS__); } }while(0)


using namespace android;
using namespace NSCam;

class ObjectTracking : public IObjectTracking
{
    friend class IObjectTracking;
private:
    ObjectTracking();
    virtual ~ObjectTracking();
    virtual void Init(
        const NSCam::MSize &ImageSize,
        const NSCam::MPoint &InitialPoint);

    virtual int Run(
        const unsigned char *Y,
        const unsigned char *V,
        const unsigned char *U,
        NSCam::MRect *pResult);

private:
    unsigned char   *mWorkingBuffer;
    MUINT32         mWorkingBufferSize;
    MTKOT           *mpOT;
    MTKOTProcInfo   mOTProcInfo;
    MTKOTEnvInfo    mOTEnvInfo;
    MTKOTTuningPara mOTTuningPara;
    int             mLoseTrackingFlag;

    //Non-Customer Prarmeters
    const int First_Frame_Numiter_shape_I = 6;
    const int MaxObjHalfSize = 85;
    const int MinObjHalfSize = 12;
    const int CrSch_StepSize_I = 0;
    const float ARFA = 0.0;
    const int only_reshape_at_begining = 0;
    const int IniwinW = 15;
    const int IniwinH = 15;
    const int incre = 7;
    const int Ob_BoundaryMargin = 21;
    const int Tsmth_ChangeStepSize_i = 2;
    const int Numiter_position_I = 5;
    const int Numiter_shape_I = 1;
    const int Numiter_shape_F = 1;
    const float LtOcOb_ColorSimilarity_TH = 0.45;
    const int StartfrmIdx = 0;
    const float AreaParam_sigma = 3.0;
    const float COARSE_SEARCH_NON_CENTOR_TH = 1.0;
    const int EIS_GMV_CTRL = 0;
    const int KmF_SkipFrm = 5;
    const int KmF_TotalFrm = 0;
    const int ColorSpace = 0;
    const int Tsmth_frmNum = 7;
    const int OBLoseTrackingFrm = 60;
    const int OCLoseTrackingFrm = 60;
    const int LightResistance = 1;
};

sp<IObjectTracking> IObjectTracking::creatInstance()
{
    OT_LOGD();
    return new ObjectTracking();
}

IObjectTracking::~IObjectTracking()
{
}

ObjectTracking::ObjectTracking() :
    mWorkingBuffer(NULL),
    mWorkingBufferSize(0),
    mpOT(NULL),
    mLoseTrackingFlag(0)
{
    OT_LOGI();
    Init(MSize(320, 240), MPoint(160, 120));
}

ObjectTracking::~ObjectTracking()
{
    OT_LOGI();
    mpOT->OTExit();
    mpOT->destroyInstance();
    mpOT = NULL;
    delete[] mWorkingBuffer;
    mWorkingBuffer = NULL;
}

void ObjectTracking::Init(const NSCam::MSize &ImageSize, const NSCam::MPoint &InitialPoint)
{
    mOTProcInfo.SrcImgWidth     = ImageSize.w;
    mOTProcInfo.SrcImgHeight    = ImageSize.h;

    if (mpOT) {
        OT_LOGD("de-init previous");
        mpOT->OTExit();
        mpOT->destroyInstance();
        mpOT = NULL;
    }

    OT_LOGI("init img=(%3d x %3d) xy=(%3d,%3d)",
        ImageSize.w,
        ImageSize.h,
        InitialPoint.x,
        InitialPoint.y);
    mpOT = MTKOT::createInstance(DRV_OT_OBJ_SW_SINGLE);
    assert(mpOT!=NULL);

    MTKOTGetProcInfo OTGetProcInfo;
    MRESULT Retcode = mpOT->OTFeatureCtrl(MTKOT_FEATURE_GET_PROC_INFO, &mOTProcInfo, &OTGetProcInfo);

    if (mWorkingBufferSize != OTGetProcInfo.WorkingBufferSize) {
        if (mWorkingBuffer)
            delete[] mWorkingBuffer;
        mWorkingBuffer = new unsigned char[OTGetProcInfo.WorkingBufferSize];
        assert(mWorkingBuffer != NULL);
        mWorkingBufferSize = OTGetProcInfo.WorkingBufferSize;
    }

    mOTEnvInfo.SrcImgWidth      = ImageSize.w;
    mOTEnvInfo.SrcImgHeight     = ImageSize.h;
    mOTEnvInfo.SrcImgFormat     = MTKOT_IMAGE_YUV444;
    mOTEnvInfo.WorkingBufAddr   = mWorkingBuffer;
    mOTEnvInfo.WorkingBufSize   = OTGetProcInfo.WorkingBufferSize;
    mOTEnvInfo.DebugInfoFlag    = 0;
    mOTEnvInfo.pTuningPara      = &mOTTuningPara;

    //Customer Parameters
    OT_Customize_PARA OTCustomData;
    get_ot_CustomizeData(&OTCustomData);
    mOTEnvInfo.pTuningPara->OBLoseTrackingFrm   = OTCustomData.OBLoseTrackingFrm;
    mOTEnvInfo.pTuningPara->OCLoseTrackingFrm   = OTCustomData.OCLoseTrackingFrm;
    mOTEnvInfo.pTuningPara->LtOcObTH            = OTCustomData.LtOcOb_ColorSimilarity_TH;
    mOTEnvInfo.pTuningPara->ARFA                = OTCustomData.ARFA;
    mOTEnvInfo.pTuningPara->LComputeShapeF      = OTCustomData.Numiter_shape_F;
    mOTEnvInfo.pTuningPara->LightResistance     = OTCustomData.LightResistance;
    mOTEnvInfo.pTuningPara->MaxObjHalfSize      = OTCustomData.MaxObjHalfSize;
    mOTEnvInfo.pTuningPara->MinObjHalfSize      = OTCustomData.MinObjHalfSize;
    mOTEnvInfo.pTuningPara->IniWinW             = OTCustomData.IniwinW;
    mOTEnvInfo.pTuningPara->IniWinH             = OTCustomData.IniwinH;

    //Non-Customer Prarmeters
    mOTEnvInfo.pTuningPara->FirstFrameLComputeShape = First_Frame_Numiter_shape_I;
    mOTEnvInfo.pTuningPara->DisplacementRange       = CrSch_StepSize_I;
    mOTEnvInfo.pTuningPara->OnlyReshapeAtBeginning  = only_reshape_at_begining;
    mOTEnvInfo.pTuningPara->Incre                   = incre;
    mOTEnvInfo.pTuningPara->OBBoundaryMargin        = Ob_BoundaryMargin;
    mOTEnvInfo.pTuningPara->TsmthChangeStepSize     = Tsmth_ChangeStepSize_i;
    mOTEnvInfo.pTuningPara->LComputePositionI       = Numiter_position_I;
    mOTEnvInfo.pTuningPara->LComputeShapeI          = Numiter_shape_I;
    mOTEnvInfo.pTuningPara->TsmthfrmNum             = Tsmth_frmNum;

    Retcode = mpOT->OTInit(&mOTEnvInfo, 0);

    mOTProcInfo.InitargetFlag       = true;
    mOTProcInfo.InitargetCenterX    = InitialPoint.x;
    mOTProcInfo.InitargetCenterY    = InitialPoint.y;

    mLoseTrackingFlag = 0;
}

int ObjectTracking::Run(
    const unsigned char *Y,
    const unsigned char *V,
    const unsigned char *U,
    MRect *pResult
)
{
    int score;
    MTKOTResultInfo OTResultInfo;

    mOTProcInfo.SrcImgChannel1 = (MUINT8*)Y;
    mOTProcInfo.SrcImgChannel2 = (MUINT8*)V;
    mOTProcInfo.SrcImgChannel3 = (MUINT8*)U;

    MRESULT Retcode = mpOT->OTFeatureCtrl(MTKOT_FEATURE_SET_PROC_INFO, &mOTProcInfo, 0);
    Retcode = mpOT->OTMain();
    Retcode = mpOT->OTFeatureCtrl(MTKOT_FEATURE_GET_RESULT, 0, &OTResultInfo);

    if (OTResultInfo.LoseTrackingFlag == 1) {
        score = 0;
        if (mLoseTrackingFlag == 0) {
            OT_LOGI("score=  0, lose track");
            mLoseTrackingFlag = 1;
        }
    } else {
        mLoseTrackingFlag = 0;
        pResult->p.x = OTResultInfo.SmoothX0;
        pResult->p.y = OTResultInfo.SmoothY0;
        pResult->s.w = OTResultInfo.SmoothX1 - OTResultInfo.SmoothX0;
        pResult->s.h = OTResultInfo.SmoothY1 - OTResultInfo.SmoothY0;

        if(OTResultInfo.OBCondition || OTResultInfo.OCCondition || OTResultInfo.LTCondition)
            score = 50;
        else
            score = 100;

        OT_LOGD("score=%3d, xy=(%3d,%3d) size=(%3d,%3d)",
            score,
            pResult->p.x,
            pResult->p.y,
            pResult->s.w,
            pResult->s.h);
    }
    mOTProcInfo.InitargetFlag = false;
    return score;
}
