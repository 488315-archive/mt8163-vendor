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
#define LOG_TAG "capturenr"
//
/*******************************************************************************
*
********************************************************************************/
#include <Log.h>
#define MY_LOGV(fmt, arg...)    CAM_LOGV("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)    CAM_LOGD("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)    CAM_LOGI("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)    CAM_LOGW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)    CAM_LOGE("[%s] " fmt, __FUNCTION__, ##arg)
//
#include <common.h>
//
#include <utils/StrongPointer.h>
using namespace android;
//
//
#include <capturenr.h>
#include <libnr/MTKNR.h>
//
#include <utils/Mutex.h>

#include <sys/mman.h>
//
#include <camera_custom_capture_nr.h>
// custom tuning
#include <IHalSensor.h>
#include <camera_custom_nvram.h>
#include <nvbuf_util.h>
//#include <isp_tuning.h>
//
#include <aaa_types.h>
#include <aaa_log.h>
#include <isp_tuning/isp_tuning.h>
#include <isp_tuning/isp_tuning_cam_info.h>
#include <isp_tuning/isp_tuning_idx.h>
#include <isp_tuning/isp_tuning_custom.h>
#include <isp_tuning/isp_tuning_custom_swnr.h>
#include <lib3a/isp_interpolation.h>
//
#include <cutils/properties.h>
//
using namespace NSCam;
/*******************************************************************************
*
********************************************************************************/
// these assertions are to make sure nvram is large enough
static_assert(
        sizeof(NVRAM_CAMERA_FEATURE_SWNR_STRUCT) >= sizeof(SWNR_PARAM) + sizeof(FSWNR_PARAM),
        "should check NVRAM_CAMERA_FEATURE_SWNR_STRUCT"
        );

static_assert(
        sizeof(NVRAM_CAMERA_FEATURE_SWNR_STRUCT) >=
        (sizeof(NRTuningInfo) + offsetof(NR2TuningInfo, reserved) +
        2 * (sizeof(CCRTuningInfo) + sizeof(HFGTuningInfo))),
        "[FIXME] should modify swnr nvram struct"
        );

static_assert(
        sizeof(NR_PARAM) == sizeof(NRTuningInfo),
        "[FIXME] struct size is not correct"
        );

static_assert(
        sizeof(NR2_PARAM) == sizeof(NR2TuningInfo) - member_size(NR2TuningInfo, reserved),
        "[FIXME] struct size is not correct"
        );

static_assert(
        sizeof(HFG_PARAM) == sizeof(HFGTuningInfo),
        "[FIXME] struct size is not correct"
        );

static_assert(
        sizeof(CCR_PARAM) == sizeof(CCRTuningInfo),
        "[FIXME] struct size is not correct"
        );

#if 0 //get the size of NVRAM_CAMERA_FEATURE_STRUCT
template<int s> struct test;
test<sizeof(NVRAM_CAMERA_FEATURE_STRUCT)> tt;
#endif

/*******************************************************************************
*
********************************************************************************/
class SwNRParam
{
    public:
        struct Params
        {
            MUINT32 iso;
            MBOOL   isMfll;
        };

    public:
        SwNRParam(MUINT32 openId);
        ~SwNRParam();

    public:
        MVOID      setParam(Params const& rParam) { mParams = rParam; }
        MINT32     getNRType() const { return mNRType; }
        MBOOL      getTuning(NRTuningInfo* pNR, HFGTuningInfo* pHFG, CCRTuningInfo* pCCR);
        MBOOL      getTuning(NR2TuningInfo* pNR, HFGTuningInfo* pHFG, CCRTuningInfo* pCCR);
        //
        MBOOL      update();
        //
        // dump params
        MBOOL      dumpParamsToFile(char * const filename);

    protected:

        MBOOL      getTuningFromNvram(
                        MBOOL isMfll,
                        MUINT32 idx,
                        MVOID** ppNvram
                   );
        MUINT32    getIso() const { return mParams.iso; }
        MBOOL      isMfll() const { return mParams.isMfll; }

    private:
        MUINT32 const   mOpenId;
        Params          mParams;
        //
        MINT32          mNRType;
        //
        MVOID*          mpNvramData;
};


/*******************************************************************************
*
********************************************************************************/
inline MINT32 generateSeed() { return (MINT32)::systemTime(); }

#define DUMP_PARAM(fp, fmt, arg...)            \
    do {                                       \
        if( fp ) fprintf(fp, fmt "\n", ##arg); \
        else MY_LOGD(fmt, ##arg);              \
    } while(0)

MVOID
dumpParams(NRTuningInfo* pNR, FILE* fp = NULL)
{
    if( ! pNR ) {
        MY_LOGE("null");
        return;
    }
#define DUMP_NR(param)                               \
    do {                                             \
        DUMP_PARAM(fp, "%s:%d", #param, pNR->param); \
    } while(0)
    DUMP_NR(ANR_Y_LUMA_SCALE_RANGE);
    DUMP_NR(ANR_C_CHROMA_SCALE);
    DUMP_NR(ANR_Y_SCALE_CPY0);
    DUMP_NR(ANR_Y_SCALE_CPY1);
    DUMP_NR(ANR_Y_SCALE_CPY2);
    DUMP_NR(ANR_Y_SCALE_CPY3);
    DUMP_NR(ANR_Y_SCALE_CPY4);
    DUMP_NR(ANR_Y_CPX1);
    DUMP_NR(ANR_Y_CPX2);
    DUMP_NR(ANR_Y_CPX3);
    DUMP_NR(ANR_CEN_GAIN_LO_TH);
    DUMP_NR(ANR_CEN_GAIN_HI_TH);
    DUMP_NR(ANR_PTY_GAIN_TH);
    DUMP_NR(ANR_KSIZE_LO_TH);
    DUMP_NR(ANR_KSIZE_HI_TH);
    DUMP_NR(ANR_KSIZE_LO_TH_C);
    DUMP_NR(ANR_KSIZE_HI_TH_C);
    DUMP_NR(ITUNE_ANR_PTY_STD);
    DUMP_NR(ITUNE_ANR_PTU_STD);
    DUMP_NR(ITUNE_ANR_PTV_STD);
    DUMP_NR(ANR_ACT_TH_Y);
    DUMP_NR(ANR_ACT_BLD_BASE_Y);
    DUMP_NR(ANR_ACT_BLD_TH_Y);
    DUMP_NR(ANR_ACT_SLANT_Y);
    DUMP_NR(ANR_ACT_TH_C);
    DUMP_NR(ANR_ACT_BLD_BASE_C);
    DUMP_NR(ANR_ACT_BLD_TH_C);
    DUMP_NR(ANR_ACT_SLANT_C);
    DUMP_NR(RADIUS_H);
    DUMP_NR(RADIUS_V);
    DUMP_NR(RADIUS_H_C);
    DUMP_NR(RADIUS_V_C);
    DUMP_NR(ANR_PTC_HGAIN);
    DUMP_NR(ANR_PTY_HGAIN);
    DUMP_NR(ANR_LPF_HALFKERNEL);
    DUMP_NR(ANR_LPF_HALFKERNEL_C);
    DUMP_NR(ANR_ACT_MODE);
    DUMP_NR(ANR_LCE_SCALE_GAIN);
    DUMP_NR(ANR_LCE_C_GAIN);
    DUMP_NR(ANR_LCE_GAIN0);
    DUMP_NR(ANR_LCE_GAIN1);
    DUMP_NR(ANR_LCE_GAIN2);
    DUMP_NR(ANR_LCE_GAIN3);
    DUMP_NR(ANR_MEDIAN_LOCATION);
    DUMP_NR(ANR_CEN_X);
    DUMP_NR(ANR_CEN_Y);
    DUMP_NR(ANR_R1);
    DUMP_NR(ANR_R2);
    DUMP_NR(ANR_R3);
    DUMP_NR(LUMA_ON_OFF);
#undef DUMP_NR
}


MVOID
dumpParams(NR2TuningInfo* pNR, FILE* fp = NULL)
{
    if( ! pNR ) {
        MY_LOGE("null");
        return;
    }
#define DUMP_NR2(param)                              \
    do {                                             \
        DUMP_PARAM(fp, "%s:%d", #param, pNR->param); \
    } while(0)
    DUMP_NR2(NR_K);
    DUMP_NR2(NR_S);
    DUMP_NR2(NR_SD);
    DUMP_NR2(NR_BLD_W);
    DUMP_NR2(NR_BLD_TH);
    DUMP_NR2(NR_SMTH);
#undef DUMP_NR2
}


MVOID
dumpParams(HFGTuningInfo* pHFG, FILE* fp = NULL)
{
    if( ! pHFG ) {
        MY_LOGE("null");
        return;
    }
#define DUMP_HFG(param)                               \
    do {                                              \
        DUMP_PARAM(fp, "%s:%d", #param, pHFG->param); \
    } while(0)
    DUMP_HFG(HFG_ENABLE);
    DUMP_HFG(HFG_GSD);
    DUMP_HFG(HFG_SD0);
    DUMP_HFG(HFG_SD1);
    DUMP_HFG(HFG_SD2);
    DUMP_HFG(HFG_TX_S);
    DUMP_HFG(HFG_LCE_LINK_EN);
    DUMP_HFG(HFG_LUMA_CPX1);
    DUMP_HFG(HFG_LUMA_CPX2);
    DUMP_HFG(HFG_LUMA_CPX3);
    DUMP_HFG(HFG_LUMA_CPY0);
    DUMP_HFG(HFG_LUMA_CPY1);
    DUMP_HFG(HFG_LUMA_CPY2);
    DUMP_HFG(HFG_LUMA_CPY3);
    DUMP_HFG(HFG_LUMA_SP0);
    DUMP_HFG(HFG_LUMA_SP1);
    DUMP_HFG(HFG_LUMA_SP2);
    DUMP_HFG(HFG_LUMA_SP3);
#undef DUMP_HFG
}


MVOID
dumpParams(CCRTuningInfo* pCCR, FILE* fp = NULL)
{
    if( ! pCCR ) {
        MY_LOGE("null");
        return;
    }
#define DUMP_CCR(param)                               \
    do {                                              \
        DUMP_PARAM(fp, "%s:%d", #param, pCCR->param); \
    } while(0)
    DUMP_CCR(CCR_ENABLE);
    DUMP_CCR(CCR_CEN_U);
    DUMP_CCR(CCR_CEN_V);
    DUMP_CCR(CCR_Y_CPX1);
    DUMP_CCR(CCR_Y_CPX2);
    DUMP_CCR(CCR_Y_CPY1);
    DUMP_CCR(CCR_Y_SP1);
    DUMP_CCR(CCR_UV_X1);
    DUMP_CCR(CCR_UV_X2);
    DUMP_CCR(CCR_UV_X3);
    DUMP_CCR(CCR_UV_GAIN1);
    DUMP_CCR(CCR_UV_GAIN2);
    DUMP_CCR(CCR_UV_GAIN_SP1);
    DUMP_CCR(CCR_UV_GAIN_SP2);
    DUMP_CCR(CCR_Y_CPX3);
    DUMP_CCR(CCR_Y_CPY0);
    DUMP_CCR(CCR_Y_CPY2);
    DUMP_CCR(CCR_Y_SP0);
    DUMP_CCR(CCR_Y_SP2);
    DUMP_CCR(CCR_UV_GAIN_MODE);
    DUMP_CCR(CCR_MODE);
    DUMP_CCR(CCR_OR_MODE);
    DUMP_CCR(CCR_HUE_X1);
    DUMP_CCR(CCR_HUE_X2);
    DUMP_CCR(CCR_HUE_X3);
    DUMP_CCR(CCR_HUE_X4);
    DUMP_CCR(CCR_HUE_SP1);
    DUMP_CCR(CCR_HUE_SP2);
    DUMP_CCR(CCR_HUE_GAIN1);
    DUMP_CCR(CCR_HUE_GAIN2);
#undef DUMP_CCR
}
#undef DUMP_PARAM

SwNRParam::
SwNRParam(MUINT32 openId)
    : mOpenId(openId)
{
    mpNvramData = (NVRAM_CAMERA_FEATURE_SWNR_STRUCT*)malloc(sizeof(NVRAM_CAMERA_FEATURE_SWNR_STRUCT));
}


SwNRParam::
~SwNRParam()
{
    if( mpNvramData )
        free(mpNvramData);
}


MBOOL
SwNRParam::
getTuningFromNvram(MBOOL isMfll, MUINT32 idx, MVOID** ppNvram)
{
    if( idx >= eNUM_OF_ISO_IDX )
    {
        MY_LOGE("wrong nvram idx %d", idx);
        return MFALSE;
    }

    // load some setting from nvram
    int err;
    NVRAM_CAMERA_FEATURE_STRUCT* pNvram;
    MUINT sensorDev = IHalSensorList::get()->querySensorDevIdx(mOpenId);
    //
    err = NvBufUtil::getInstance().getBufAndRead(
            CAMERA_NVRAM_DATA_FEATURE, sensorDev, (void*&)pNvram);
    if( err != 0 )
    {
        MY_LOGE("getBufAndRead fail, err=%d", err);
        return MFALSE;
    }

    *ppNvram = isMfll ? &(pNvram->nr_mfll[idx]) : &(pNvram->nr_normal[idx]);
    return MTRUE;
}


MBOOL
SwNRParam::
update()
{
    class ScopedHelper
    {
    public:
                ScopedHelper(MINT32 const openId) : pIspTuningCustom(NULL) {
                    IHalSensorList* const pHalSensorList = IHalSensorList::get();
                    sensorDev = pHalSensorList->querySensorDevIdx(openId);
                    MUINT32 u4SensorID;
                    {
                        SensorStaticInfo rSensorStaticInfo;
                        switch  ( sensorDev )
                        {
                            case SENSOR_DEV_MAIN:
                                pHalSensorList->querySensorStaticInfo(
                                        NSCam::SENSOR_DEV_MAIN, &rSensorStaticInfo);
                                break;
                            case SENSOR_DEV_SUB:
                                pHalSensorList->querySensorStaticInfo(
                                        NSCam::SENSOR_DEV_SUB, &rSensorStaticInfo);
                                break;
                            case SENSOR_DEV_MAIN_2:
                                pHalSensorList->querySensorStaticInfo(
                                        NSCam::SENSOR_DEV_MAIN_2, &rSensorStaticInfo);
                                break;
                            default:    //  Shouldn't happen.
                                MY_ERR("Invalid sensor device: %d", sensorDev);
                                break;
                        }
                        u4SensorID = rSensorStaticInfo.sensorDevID;
                        MY_LOGD("sensorDev(%d), u4SensorID(%d)", sensorDev, u4SensorID);
                    }
                    //
                    pIspTuningCustom = IspTuningCustom::createInstance(
                            static_cast<NSIspTuning::ESensorDev_T>(sensorDev), u4SensorID
                            );
                }
                ~ScopedHelper() {
                    if( pIspTuningCustom ) pIspTuningCustom->destroyInstance();
                }
    public:
        IspTuningCustom*    get() const { return pIspTuningCustom; };
        MUINT               getSensorDev() const { return sensorDev; }

    private:
        MUINT               sensorDev;
        IspTuningCustom*    pIspTuningCustom;

    } ispCustom(mOpenId);
    //
    if( ! ispCustom.get() ) {
        MY_LOGE("cannnot get ispTuningCustom");
        return MFALSE;
    }
    //
    // 1. update swnr type
    mNRType = get_swnr_type(ispCustom.getSensorDev());
    //
    { // debug
        MINT32 type = property_get_int32("debug.swnr.type", -1);
        if( type != -1 ) {
            MY_LOGD("force use swnr: %s", type == eSWNRType_SW ? "normal" : "fast");
            mNRType = type;
        }
    }
    //
    if( mNRType < 0 || mNRType >= eSWNRType_NUM ) {
        MY_LOGE("incorrect swnr type %d", mNRType);
        return MFALSE;
    }
    //
    // 2. read from nvram & smooth
    if( is_to_invoke_swnr_interpolation(isMfll(), getIso()) )
    {

        EIndex_ISO idx_low  = ispCustom.get()->map_ISO_value_to_lower_index(getIso(), NSIspTuning::EIspProfile_Capture);
        EIndex_ISO idx_high = ispCustom.get()->map_ISO_value_to_upper_index(getIso(), NSIspTuning::EIspProfile_Capture);
        MUINT32 iso_low     = ispCustom.get()->map_ISO_index_to_value(idx_low, NSIspTuning::EIspProfile_Capture);
        MUINT32 iso_high    = ispCustom.get()->map_ISO_index_to_value(idx_high, NSIspTuning::EIspProfile_Capture);
        //
        MY_LOGD("%d: iso %d, isMfll %d, idx low/high %d/%d",
                mOpenId,
                getIso(),
                isMfll(),
                idx_low,
                idx_high
               );
        //
        MVOID* pNvramLow = NULL;
        MVOID* pNvramHigh = NULL;
        if( !getTuningFromNvram(isMfll(), idx_low, &pNvramLow) ||
            !getTuningFromNvram(isMfll(), idx_high, &pNvramHigh) )
        {
            MY_LOGE("update from nvram failed");
            return MFALSE;
        }
        //
        // interpolation
        auto interpolation_func = (mNRType == eSWNRType_SW) ? SmoothSWNR : SmoothFSWNR;
        interpolation_func(
                ispCustom.get()->remap_ISO_value(getIso()),
                iso_high, iso_low,
                *reinterpret_cast<NVRAM_CAMERA_FEATURE_SWNR_STRUCT*>(pNvramHigh),
                *reinterpret_cast<NVRAM_CAMERA_FEATURE_SWNR_STRUCT*>(pNvramLow),
                *reinterpret_cast<NVRAM_CAMERA_FEATURE_SWNR_STRUCT*>(mpNvramData)
                );
    }
    else
    {

        EIndex_ISO idx = ispCustom.get()->map_ISO_value_to_index(getIso(), NSIspTuning::EIspProfile_Capture);

        MY_LOGD("%d: iso %d, isMfll %d, idx %d",
                mOpenId, getIso(), isMfll(), idx
               );

        // get nvram
        MVOID* pTuningNvram = NULL;
        if( !getTuningFromNvram(isMfll(), idx, &pTuningNvram) )
        {
            MY_LOGE("update from nvram failed");
            return MFALSE;
        }

        // update
        memcpy(mpNvramData, pTuningNvram, sizeof(NVRAM_CAMERA_FEATURE_SWNR_STRUCT) );
    }
    return MTRUE;
}


MBOOL
SwNRParam::
getTuning(NRTuningInfo* pNR, HFGTuningInfo* pHFG, CCRTuningInfo* pCCR)
{
    if( ! pNR || ! pHFG || ! pCCR ) {
        MY_LOGE("pNR(%p), pHFG(%p), pCCR(%p)", pNR, pHFG, pCCR );
        return MFALSE;
    }
    NVRAM_CAMERA_FEATURE_SWNR_STRUCT* pNvram = (NVRAM_CAMERA_FEATURE_SWNR_STRUCT*)mpNvramData;
    SWNR_PARAM* pPARAM = &(pNvram->swnr);
    //
    memcpy(pNR , &(pPARAM->NR) , sizeof(NR_PARAM));
    memcpy(pHFG, &(pPARAM->HFG), sizeof(HFG_PARAM));
    memcpy(pCCR, &(pPARAM->CCR), sizeof(CCR_PARAM));
    //
    // update some parameters
    pHFG->HFG_GSD = generateSeed();
    //
    {
        char value[PROPERTY_VALUE_MAX] = {'\0'};
        property_get("debug.swnr.enable", value, "0");
        MBOOL bDebug = atoi(value);

        if( bDebug ) {
            dumpParams(pNR);
            dumpParams(pHFG);
            dumpParams(pCCR);
        }
    }
    return MTRUE;
}


MBOOL
SwNRParam::
getTuning(NR2TuningInfo* pNR, HFGTuningInfo* pHFG, CCRTuningInfo* pCCR)
{
    if( ! pNR || ! pHFG || ! pCCR ) {
        MY_LOGE("pNR(%p), pHFG(%p), pCCR(%p)", pNR, pHFG, pCCR );
        return MFALSE;
    }
    NVRAM_CAMERA_FEATURE_SWNR_STRUCT* pNvram = (NVRAM_CAMERA_FEATURE_SWNR_STRUCT*)mpNvramData;
    FSWNR_PARAM* pPARAM = &(pNvram->fswnr);
    //
    memcpy(pNR , &(pPARAM->NR) , sizeof(NR2_PARAM));
    memcpy(pHFG, &(pPARAM->HFG), sizeof(HFG_PARAM));
    memcpy(pCCR, &(pPARAM->CCR), sizeof(CCR_PARAM));
    //
    // update some parameters
    pNR->NR_SD = generateSeed();
    pHFG->HFG_GSD = generateSeed();
    //
    {
        char value[PROPERTY_VALUE_MAX] = {'\0'};
        property_get("debug.swnr.enable", value, "0");
        MBOOL bDebug = atoi(value);

        if( bDebug ) {
            dumpParams(pNR);
            dumpParams(pHFG);
            dumpParams(pCCR);
        }
    }
    return MTRUE;
}


SwNR::
SwNR(MUINT32 const openId)
    : muOpenId(openId)
    , mpNRTuningInfo(NULL)
    , mpNR2TuningInfo(NULL)
    , mpHFGTuningInfo(NULL)
    , mpCCRTuningInfo(NULL)
    , muWorkingBufSize(0)
    , mpWorkingBuf(NULL)
{
    mpNRInitInfo   = (NRInitInfo*)malloc(sizeof(NRInitInfo));
    mpNRImageInfo  = (NRImageInfo*)malloc(sizeof(NRImageInfo));
    mpNRResultInfo = (NRResultInfo*)malloc(sizeof(NRResultInfo));
    // initial
    mpNRInitInfo->pWorkingBuff    = 0;
    mpNRInitInfo->WorkingBuffSize = 0;
    mpNRInitInfo->CoreNumber      = NR_MAX_CORE_NO;
    mpNRInitInfo->NumOfExecution  = 1;
    mpNRInitInfo->pTuningInfo     = NULL;
    mpNRInitInfo->pCCRTuningInfo  = NULL;
    mpNRInitInfo->pHFGTuningInfo  = NULL;
    mpNRInitInfo->pPerfInfo       = NULL;
}


SwNR::
~SwNR()
{
#define MY_FREE( ptr ) \
    if( ptr )          \
    {                  \
        free(ptr);     \
        ptr = NULL;    \
    }
    MY_FREE(mpNRInitInfo->pPerfInfo);
    MY_FREE(mpNRInitInfo);
    MY_FREE(mpNRTuningInfo);
    MY_FREE(mpNR2TuningInfo);
    MY_FREE(mpHFGTuningInfo);
    MY_FREE(mpCCRTuningInfo);
    MY_FREE(mpNRImageInfo);
    MY_FREE(mpNRResultInfo);
#undef MY_FREE

    if( mpWorkingBuf ) {
        munmap(mpWorkingBuf, sizeof(MUINT8) * muWorkingBufSize);
        mpWorkingBuf = NULL;
    }
}

MBOOL
SwNR::
prepareTuningInfo()
{
    MBOOL ret = MFALSE;
    //
    SwNRParam param = SwNRParam(muOpenId);
    //
    SwNRParam::Params p;
    p.iso    = mParam.iso;
    p.isMfll = mParam.isMfll;
    param.setParam(p);
    //
    if( ! param.update() ) {
        MY_LOGW("cannot update swnr param properly");
        goto lbExit;
    }
    //
    switch( param.getNRType() )
    {
        case eSWNRType_SW:
            mNRType = DRV_NR_OBJ_SW;
            //
            if( ! mpNRTuningInfo ) mpNRTuningInfo = (NRTuningInfo*)malloc(sizeof(NRTuningInfo));
            if( mpNR2TuningInfo ) { free(mpNR2TuningInfo); mpNR2TuningInfo = NULL; }
            if( ! mpHFGTuningInfo ) mpHFGTuningInfo = (HFGTuningInfo*)malloc(sizeof(HFGTuningInfo));
            if( ! mpCCRTuningInfo ) mpCCRTuningInfo = (CCRTuningInfo*)malloc(sizeof(CCRTuningInfo));
            //
            if( ! param.getTuning(mpNRTuningInfo, mpHFGTuningInfo, mpCCRTuningInfo) ) {
                MY_LOGE("getTuning failed");
                goto lbExit;
            }
            //
            mpNRInitInfo->pTuningInfo    = mpNRTuningInfo;
            mpNRInitInfo->pCCRTuningInfo = mpCCRTuningInfo;
            mpNRInitInfo->pHFGTuningInfo = mpHFGTuningInfo;
            break;
        case eSWNRType_SW2:
        default:
            mNRType = DRV_NR_OBJ_SW2;
            //
            if( mpNRTuningInfo ) { free(mpNRTuningInfo); mpNRTuningInfo = NULL; }
            if( ! mpNR2TuningInfo ) mpNR2TuningInfo = (NR2TuningInfo*)malloc(sizeof(NR2TuningInfo));
            if( ! mpHFGTuningInfo ) mpHFGTuningInfo = (HFGTuningInfo*)malloc(sizeof(HFGTuningInfo));
            if( ! mpCCRTuningInfo ) mpCCRTuningInfo = (CCRTuningInfo*)malloc(sizeof(CCRTuningInfo));
            //
            if( ! param.getTuning(mpNR2TuningInfo, mpHFGTuningInfo, mpCCRTuningInfo) ) {
                MY_LOGE("getTuning failed");
                goto lbExit;
            }
            mpNRInitInfo->pNR2TuningInfo = mpNR2TuningInfo;
            mpNRInitInfo->pCCRTuningInfo = mpCCRTuningInfo;
            mpNRInitInfo->pHFGTuningInfo = mpHFGTuningInfo;
            break;
    }
    //
    // set performance level
    {
        if( mpNRInitInfo->pPerfInfo == NULL ) {
            mpNRInitInfo->pPerfInfo = new NRPerfGear;
        }
        //
        switch(mParam.perfLevel)
        {
#define PerfCase(val, level)                             \
            case val:                                    \
                mpNRInitInfo->pPerfInfo->enable = 1;     \
                mpNRInitInfo->pPerfInfo->option = level; \
                break;
            PerfCase(eSWNRPerf_Performance_First, NR_PERF_GEAR_PERF_FIRST)
            PerfCase(eSWNRPerf_Custom0          , NR_PERF_GEAR_CUSTOM_0)
            PerfCase(eSWNRPerf_Power_First      , NR_PERF_GEAR_POWER_FIRST)
            default:
                MY_LOGW("not valid level %d", mParam.perfLevel);
                delete mpNRInitInfo->pPerfInfo;
                mpNRInitInfo->pPerfInfo = NULL;
#undef PerfCase
        }
    }
    //
    ret = MTRUE;
lbExit:
    return ret;
}


MBOOL
SwNR::
setImageInfo(IImageBuffer* pBuf)
{
    MBOOL ret = MFALSE;
    MTK_NR_IMAGE_FMT_ENUM NRFmt = NR_IMAGE_PACKET_YUY2;
    switch( pBuf->getImgFormat() )
    {
        case eImgFmt_YUY2:
            NRFmt = NR_IMAGE_PACKET_YUY2;
            break;
        case eImgFmt_I420:
            NRFmt = NR_IMAGE_YUV420;
            break;
        case eImgFmt_YV12:
            NRFmt = NR_IMAGE_YV12;
            break;
        default:
            MY_LOGE("not supported format: 0x%x", pBuf->getImgFormat() );
            goto lbExit;
    }

    // set image info
    mpNRImageInfo->Width = pBuf->getImgSize().w;
    mpNRImageInfo->Height = pBuf->getImgSize().h;
    mpNRImageInfo->pImg[0] = (void*)pBuf->getBufVA(0);
    mpNRImageInfo->pImg[1] = pBuf->getPlaneCount() > 1 ? (void*)pBuf->getBufVA(1) : (void*)0,
    mpNRImageInfo->pImg[2] = pBuf->getPlaneCount() > 2 ? (void*)pBuf->getBufVA(2) : (void*)0,

#if 0
    MY_LOG("buf %dx%d va(0x%x/0x%x/0x%x) size(%d/%d/%d)",
            mpNRImageInfo->Width, mpNRImageInfo->Height,
            mpNRImageInfo->pImg[0], mpNRImageInfo->pImg[1], mpNRImageInfo->pImg[2],
            pBuf->getBufSizeInBytes(0),
            pBuf->getPlaneCount() > 1 ? pBuf->getBufSizeInBytes(1) : 0,
            pBuf->getPlaneCount() > 2 ? pBuf->getBufSizeInBytes(2) : 0
          );

#endif
    mpNRImageInfo->ImgNum = 1;
    mpNRImageInfo->ImgFmt = NRFmt;

    ret = MTRUE;
lbExit:
    return ret;
}


MBOOL
SwNR::
allocWorkingBuf(MUINT32 size)
{
    MBOOL ret = MFALSE;
    // allocate working buffer
    if( size != muWorkingBufSize )
    {
        if( mpWorkingBuf )
            free(mpWorkingBuf);
        mpWorkingBuf = (MUINT8*)mmap(NULL, sizeof(MUINT8) * size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if( mpWorkingBuf == MAP_FAILED )
        {
            MY_LOGE("allocate working buffer failed");
            goto lbExit;
        }
        MY_LOGD("allocate working buffer %p, size %d", mpWorkingBuf, size);
        muWorkingBufSize = size;
    }

    mpNRInitInfo->pWorkingBuff = mpWorkingBuf;
    mpNRInitInfo->WorkingBuffSize = muWorkingBufSize;

    ret = MTRUE;
lbExit:
    return ret;
}


MBOOL
SwNR::
doSwNR(SWNRParam const& param, NSCam::IImageBuffer* pBuf)
{
#define CHECK_NR_RET( NR_ret )                     \
    do{                                            \
        MRESULT nr_ret = (NR_ret);                 \
        if( nr_ret != S_NR_OK ) {                  \
            MY_LOGE("NR with error 0x%x", nr_ret); \
            goto lbExit;                           \
        }                                          \
    }                                              \
    while(0);
    mParam = param;
    MY_LOGD("idx %d: iso %d perf %d, buf %p",
            muOpenId,
            mParam.iso, mParam.perfLevel,
            pBuf);
    MBOOL ret = MFALSE;
    //
    MUINT32 workingbufsize = 0;
    MTKNR* pMTKNR = NULL;

    // get tuning info
    if( !prepareTuningInfo() )
        goto lbExit;
    //
    MY_LOGD("swnr type %s", mNRType == DRV_NR_OBJ_SW ? "normal" : "fast");
    pMTKNR = MTKNR::createInstance(static_cast<DrvNRObject_e>(mNRType));
    //
    CHECK_NR_RET( pMTKNR->NRInit((MUINT32*) mpNRInitInfo, 0) );

    if( !setImageInfo(pBuf) )
        goto lbExit;

    CHECK_NR_RET(
            pMTKNR->NRFeatureCtrl(
                NR_FEATURE_GET_WORKBUF_SIZE,
                (void *)mpNRImageInfo,
                (void *)&workingbufsize)
            );

    if( !allocWorkingBuf(workingbufsize) )
        goto lbExit;

    CHECK_NR_RET(
            pMTKNR->NRFeatureCtrl(
                NR_FEATURE_SET_WORKBUF_ADDR,
                mpNRInitInfo->pWorkingBuff,
                NULL)
            );

    // add img
    CHECK_NR_RET(
            pMTKNR->NRFeatureCtrl(
                NR_FEATURE_ADD_IMG,
                (void *)mpNRImageInfo,
                NULL)
            );

    MY_LOGD("start+");
    // main
    CHECK_NR_RET( pMTKNR->NRMain() );
    MY_LOGD("start-");

    CHECK_NR_RET(
            pMTKNR->NRFeatureCtrl(
                NR_FEATURE_GET_RESULT,
                NULL,
                (void *)mpNRResultInfo)
            );

    pBuf->syncCache(eCACHECTRL_FLUSH);

    pMTKNR->NRReset();

#undef CHECK_NR_RET
    ret = MTRUE;
lbExit:
    if( pMTKNR ) {
        pMTKNR->destroyInstance(pMTKNR);
        pMTKNR = NULL;
    }

    return ret;
}


MVOID
SwNR::
dumpParam(char * const filename)
{
    // write to file
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        MY_LOGE("fopen fail: %s", filename);
        return;
    }
    //
    fprintf(fp, "swnr:%s\n\n", (mNRType == eSWNRType_SW) ? "normal" : "fast");
    //
    if( mNRType == eSWNRType_SW )
    {
        dumpParams(mpNRTuningInfo, fp);
        dumpParams(mpHFGTuningInfo, fp);
        dumpParams(mpCCRTuningInfo, fp);
    }
    else
    {
        dumpParams(mpNR2TuningInfo, fp);
        dumpParams(mpHFGTuningInfo, fp);
        dumpParams(mpCCRTuningInfo, fp);
    }
    //
    fclose(fp);
}
