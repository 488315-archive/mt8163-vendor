/*********************************************************************************************
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
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/

#define LOG_TAG "N3D_HAL"

#include <stdlib.h>     // for rand()
#include <bitset>

#include <Log.h>
#include "n3d_hal_imp.h"         // For N3D_HAL class.
#include "../inc/stereo_dp_util.h"

#include <IHalSensor.h>
#include <nvbuf_util.h>
#include <Hal3/IHal3A.h>
#include <hal/inc/camera_custom_stereo.h>  // For CUST_STEREO_* definitions.
#include <ui/GraphicBuffer.h>
#include <math.h>

#include <vsdof/hal/rapidjson/writer.h>
#include <vsdof/hal/rapidjson/stringbuffer.h>
#include <vsdof/hal/rapidjson/document.h>     // rapidjson's DOM-style API
#include <vsdof/hal/rapidjson/prettywriter.h> // for stringify JSON
#include <vsdof/hal/rapidjson/filewritestream.h>
#include <vsdof/hal/rapidjson/writer.h>

#include "lens_info.h"

#define N3D_HAL_DEBUG
const bool LOG_ENABLED = StereoSettingProvider::isLogEnabled(PERPERTY_DEPTHMAP_NODE_LOG);
const bool BENCHMARK_ENABLED = StereoSettingProvider::isProfileLogEnabled();

#ifdef N3D_HAL_DEBUG    // Enable debug log.
#if defined(__func__)
#undef __func__
#endif
#define __func__ __FUNCTION__

#ifndef GTEST
#define MY_LOGD(fmt, arg...)    CAM_LOGD("[%s]" fmt, __func__, ##arg)
#define MY_LOGI(fmt, arg...)    CAM_LOGI("[%s]" fmt, __func__, ##arg)
#define MY_LOGW(fmt, arg...)    CAM_LOGW("[%s] WRN(%5d):" fmt, __func__, __LINE__, ##arg)
#define MY_LOGE(fmt, arg...)    CAM_LOGE("[%s] %s ERROR(%5d):" fmt, __func__,__FILE__, __LINE__, ##arg)
#else
#define MY_LOGD(fmt, arg...)    printf("[D][%s]" fmt"\n", __func__, ##arg)
#define MY_LOGI(fmt, arg...)    printf("[I][%s]" fmt"\n", __func__, ##arg)
#define MY_LOGW(fmt, arg...)    printf("[W][%s] WRN(%5d):" fmt"\n", __func__, __LINE__, ##arg)
#define MY_LOGE(fmt, arg...)    printf("[E][%s] %s ERROR(%5d):" fmt"\n", __func__,__FILE__, __LINE__, ##arg)
#endif

#else   // Disable debug log.
#define MY_LOGD(a,...)
#define MY_LOGI(a,...)
#define MY_LOGW(a,...)
#define MY_LOGE(a,...)

#endif  // N3D_HAL_DEBUG

#define MY_LOGV_IF(cond, arg...)    if (cond) { MY_LOGV(arg); }
#define MY_LOGD_IF(cond, arg...)    if (cond) { MY_LOGD(arg); }
#define MY_LOGI_IF(cond, arg...)    if (cond) { MY_LOGI(arg); }
#define MY_LOGW_IF(cond, arg...)    if (cond) { MY_LOGW(arg); }
#define MY_LOGE_IF(cond, arg...)    if (cond) { MY_LOGE(arg); }

#define FUNC_START          MY_LOGD_IF(LOG_ENABLED, "+")
#define FUNC_END            MY_LOGD_IF(LOG_ENABLED, "-")

using namespace NSCam;
using namespace android;
using namespace rapidjson;
using namespace std;
using namespace StereoHAL;
using android::Mutex;           // For android::Mutex in stereo_hal.h.

/**************************************************************************
 *                      D E F I N E S / M A C R O S                       *
 **************************************************************************/
#define PROPERTY_ENABLE_VERIFY  STEREO_PROPERTY_PREFIX"enable_verify"
#define PERPERTY_ENABLE_CC      STEREO_PROPERTY_PREFIX"enable_cc"
#define PROPERTY_ALGO_BEBUG     STEREO_PROPERTY_PREFIX"dbgdump"
#define PROPERTY_NVRAM_BEBUG    STEREO_PROPERTY_PREFIX"log_nvram"
#define PROPERTY_DUMP_OUTPUT    STEREO_PROPERTY_PREFIX"dump_n3d"

const MFLOAT RRZ_CAPIBILITY = 0.25f;
const int MAX_LENS_INFO     = 20*100*2;
/**************************************************************************
 *     E N U M / S T R U C T / T Y P E D E F    D E C L A R A T I O N     *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *       P R I V A T E    F U N C T I O N    D E C L A R A T I O N        *
 **************************************************************************/
Mutex N3D_HAL_IMP::mLock;
NVRAM_CAMERA_GEOMETRY_STRUCT* N3D_HAL_IMP::m_pVoidGeoData = NULL;
MINT32 N3D_HAL_IMP::m_nvramUserCount = 0;

/**************************************************************************
 *       Public Functions                                                 *
 **************************************************************************/

N3D_HAL *
N3D_HAL::createInstance()
{
    return new N3D_HAL_IMP();
}

N3D_HAL_IMP::N3D_HAL_IMP()
    : m_eScenario(eSTEREO_SCENARIO_UNKNOWN)
    , m_pWorkBuf(NULL)
    , m_pAFTable(NULL)
    , m_pStereoDrv(NULL)
    , m_pDepthAFHAL(NULL)
    , m_main1Mask(NULL)
    , m_main2Mask(NULL)
    , m_stereoExtraData(NULL)
{
    m_pStereoDrv = MTKStereoKernel::createInstance();
    _loadLensInfo();
    _loadNVRAM();
}

N3D_HAL_IMP::~N3D_HAL_IMP()
{
    N3DHALSaveNVRAM();

    if(m_main1Mask) {
        delete [] m_main1Mask;
        m_main1Mask = NULL;
    }

    if(m_main2Mask) {
        delete [] m_main2Mask;
        m_main2Mask = NULL;
    }

    if(m_pDepthAFHAL) {
        m_pDepthAFHAL->destroyInstance();
        m_pDepthAFHAL = NULL;
    }

    if(m_pStereoDrv) {
        delete m_pStereoDrv;
        m_pStereoDrv = NULL;
    }
}

bool
N3D_HAL_IMP::N3DHALInit(N3D_HAL_INIT_PARAM &n3dInitParam)
{
    m_eScenario = n3dInitParam.eScenario;
    switch(n3dInitParam.eScenario) {
        case eSTEREO_SCENARIO_CAPTURE:
            m_algoInitInfo.scenario = STEREO_KERNEL_SCENARIO_IMAGE_CAPTURE;
            break;
        case eSTEREO_SCENARIO_PREVIEW:
        case eSTEREO_SCENARIO_RECORD:
            // m_algoInitInfo.scenario = STEREO_KERNEL_SCENARIO_IMAGE_PREVIEW;
            m_algoInitInfo.scenario = STEREO_KERNEL_SCENARIO_VIDEO_RECORD;
            break;
        default:
            break;
    }

    if(eSTEREO_SCENARIO_CAPTURE == m_eScenario) {
        //init m_main1Mask
        StereoArea areaMask = StereoSizeProvider::getInstance()->getBufferSize(E_MASK_M_Y, m_eScenario);
        MUINT32 length = areaMask.size.w * areaMask.size.h;
        if(length > 0) {
            if(NULL == m_main1Mask) {
                m_main1Mask = new MUINT8[length];
                ::memset(m_main1Mask, 0, sizeof(MUINT8)*length);
                MUINT8 *startPos = m_main1Mask + areaMask.startPt.x+areaMask.size.w*areaMask.startPt.y;
                const MUINT32 END_Y = areaMask.contentSize().h;
                const MUINT32 CONTENT_W = areaMask.contentSize().w * sizeof(MUINT8);
                for(unsigned int y = 0; y < END_Y; y++) {
                    ::memset(startPos, 0xFF, CONTENT_W);
                    startPos += areaMask.size.w;
                }
            }
        } else {
            MY_LOGE("Size of MASK_M_Y is 0");
            return false;
        }
    }

    // ALGORITHM INPUT and SbS OUTPUT
    Pass2SizeInfo pass2SizeInfo;
    StereoSizeProvider::getInstance()->getPass2SizeInfo(PASS2A_2, m_eScenario, pass2SizeInfo);
    m_algoInitInfo.img_src_wd = pass2SizeInfo.areaWDMA.size.w;
    m_algoInitInfo.img_src_ht = pass2SizeInfo.areaWDMA.size.h;
    m_algoInitInfo.img_inp_wd = m_algoInitInfo.img_src_wd;
    m_algoInitInfo.img_inp_ht = m_algoInitInfo.img_src_ht;

    MSize szOutput = StereoSizeProvider::getInstance()->getBufferSize(E_MV_Y, m_eScenario);
    m_algoInitInfo.img_out_wd = szOutput.w;
    m_algoInitInfo.img_out_ht = szOutput.h;

    // HWFE INPUT - the actual size for HWFE (after SRZ)
    m_algoInitInfo.geo_info  = n3dInitParam.fefmRound;   //N3D_HAL_INIT_PARAM.fefmRound
    m_algoInitInfo.geo_info |= 3<<2;
    _initN3DGeoInfo(m_algoInitInfo.geo_img);              //FEFM setting

    // COLOR CORRECTION INPUT
    _initCCImgInfo(m_algoInitInfo.pho_img);       //settings of main = auxi

    // Learning
    StereoSettingProvider::getStereoCameraFOV(m_algoInitInfo.hfov_main, m_algoInitInfo.hfov_auxi);
    m_algoInitInfo.baseline         = getStereoBaseline(STEREO_SENSOR_PROFILE_REAR_REAR);
    m_algoInitInfo.system_cfg       = 35;   //00100011
    if( checkStereoProperty(PROPERTY_ENABLE_VERIFY) > 0 )
    {
        m_algoInitInfo.system_cfg  |= (1<<2);
    }

    if(STEREO_KERNEL_SCENARIO_IMAGE_CAPTURE == m_algoInitInfo.scenario) {
        m_algoInitInfo.system_cfg  |= (1<<3);  //enable GPU
    }

    if( checkStereoProperty(PERPERTY_ENABLE_CC, false) != 0 ) {
        m_algoInitInfo.system_cfg  |= (1<<4);
    }

    if( eRotate_90  == StereoSettingProvider::getModuleRotation() ||
        eRotate_270 == StereoSettingProvider::getModuleRotation() )
    {
        m_algoInitInfo.system_cfg  |= (1<<6);
    }

    if(StereoSettingProvider::getSensorRelativePosition()) {
        m_algoInitInfo.system_cfg |= (1<<7);
    }

    //Get min/max dac
    const int DEFAULT_DAC = 213;
    m_algoInitInfo.dac_start = 0;
    const char *HAL3A_QUERY_NAME = "MTKStereoCamera";
    int32_t main1Idx, main2Idx;
    StereoSettingProvider::getStereoSensorIndex(main1Idx, main2Idx);
    IHal3A *pHal3A = IHal3A::createInstance(IHal3A::E_Camera_3, main1Idx, HAL3A_QUERY_NAME);
    if(NULL == pHal3A) {
        MY_LOGE("Cannot get 3A HAL");
    } else {
        pHal3A->send3ACtrl(NS3Av3::E3ACtrl_GetAFDAFTable, (MUINTPTR)&m_pAFTable, 0);

        if(NULL == m_pAFTable) {
            m_algoInitInfo.dac_start = DEFAULT_DAC;
            MY_LOGE("Cannot get AF table");
        } else {
            //Since af_mgr::init may run later, we have to wait for it
            m_pAFTable->is_daf_run = 1;
            for(int nTimes = 10; nTimes > 0; nTimes--) {
                m_algoInitInfo.dac_start = m_pAFTable->af_dac_min;
                if (0 == m_algoInitInfo.dac_start) {
                    MY_LOGD("Waiting for af_dac_min...");
                    usleep(20 * 1000);
                } else {
                    break;
                }
            }
        }

        pHal3A->destroyInstance(HAL3A_QUERY_NAME);
    }

    if (0 == m_algoInitInfo.dac_start) {
        m_algoInitInfo.dac_start = DEFAULT_DAC;
        MY_LOGE("Cannot get af_dac_min, set min dac to %d", m_algoInitInfo.dac_start);
    }

    // Learning Coordinates RE-MAPPING
    _getStereoRemapInfo(m_algoInitInfo.flow_main, m_algoInitInfo.flow_auxi, m_eScenario);

#if 1//(1 == DEPTH_AF_SUPPORTED)
    if(NULL == m_pDepthAFHAL &&
       eSTEREO_SCENARIO_CAPTURE != m_eScenario)
    {
        m_pDepthAFHAL = StereoDepthHal::createInstance();    // init() has already run inside createInstance()

        if(m_pDepthAFHAL) {
            STEREODEPTH_HAL_INIT_PARAM_STRUCT stStereodepthHalInitParam;
            stStereodepthHalInitParam.stereo_fov_main       = m_algoInitInfo.hfov_main;
            stStereodepthHalInitParam.stereo_fov_main2      = m_algoInitInfo.hfov_auxi;
            stStereodepthHalInitParam.stereo_baseline       = m_algoInitInfo.baseline;
            stStereodepthHalInitParam.stereo_pxlarr_width   = m_algoInitInfo.flow_main.pixel_array_width;
            stStereodepthHalInitParam.stereo_pxlarr_height  = m_algoInitInfo.flow_main.pixel_array_height;
            stStereodepthHalInitParam.stereo_main12_pos     = StereoSettingProvider::getSensorRelativePosition();
            stStereodepthHalInitParam.pNvRamDataArray       = m_pVoidGeoData->StereoNvramData.DepthAfData;
            m_pDepthAFHAL->StereoDepthInit(&stStereodepthHalInitParam);
        }
    }
#endif

    MRESULT err = 0;
    err = m_pStereoDrv->StereoKernelFeatureCtrl(STEREO_KERNEL_FEATURE_GET_DEFAULT_TUNING, NULL, &m_algoTuningInfo);
    if (err) {
        MY_LOGE("StereoKernelFeatureCtrl(GET_DEFAULT_TUNING) fail. error code: %d.", err);
        return false;
    } else {
        m_algoInitInfo.ptuning_para = &m_algoTuningInfo;
    }

    err = m_pStereoDrv->StereoKernelInit(&m_algoInitInfo);
    if (err) {
        MY_LOGE("Init N3D algo failed(err %d)", err);
        return false;
    }

    // OUTPUT after Initialization
    err = m_pStereoDrv->StereoKernelFeatureCtrl(STEREO_KERNEL_FEATURE_GET_WORK_BUF_INFO, NULL,
                                                &m_algoInitInfo.working_buffer_size);
    if(err) {
        MY_LOGE("Fail to get working buffer size");
        return false;
    }

    MY_LOGD_IF(LOG_ENABLED, "Init working buf, size: %d", m_algoInitInfo.working_buffer_size);
    if(!_initWorkingBuffer(m_algoInitInfo.working_buffer_size)) {
        return false;
    }

    _dumpInitInfo(m_algoInitInfo);

    return true;
}

bool
N3D_HAL_IMP::N3DHALRun(N3D_HAL_PARAM &n3dParams, N3D_HAL_OUTPUT &n3dOutput)
{
    if(eSTEREO_SCENARIO_CAPTURE == m_eScenario) {
        MY_LOGW("Cannot run capture here");
        return false;
    }

    ENUM_STEREO_SCENARIO scenario = (n3dParams.eisData.isON) ? eSTEREO_SCENARIO_RECORD : eSTEREO_SCENARIO_PREVIEW;
    Pass2SizeInfo pass2SizeInfo;
    StereoSizeProvider::getInstance()->getPass2SizeInfo(PASS2A_2, scenario, pass2SizeInfo);
    if(m_algoInitInfo.img_src_wd != pass2SizeInfo.areaWDMA.size.w ||
       m_algoInitInfo.img_src_ht != pass2SizeInfo.areaWDMA.size.h)
    {
        m_algoInitInfo.img_src_wd = pass2SizeInfo.areaWDMA.size.w;
        m_algoInitInfo.img_src_ht = pass2SizeInfo.areaWDMA.size.h;
        m_algoInitInfo.img_inp_wd = m_algoInitInfo.img_src_wd;
        m_algoInitInfo.img_inp_ht = m_algoInitInfo.img_src_ht;

        MRESULT err = m_pStereoDrv->StereoKernelInit(&m_algoInitInfo);
        if (err) {
            MY_LOGE("Init N3D algo failed(err %d)", err);
            return false;
        } else {
            MY_LOGD("Change size: %dx%d", m_algoInitInfo.img_src_wd, m_algoInitInfo.img_src_ht);
            _dumpInitInfo(m_algoInitInfo);
        }
    }

    _setN3DParams(n3dParams, n3dOutput);
    _runN3D(n3dOutput);

#if 1//(1 == DEPTH_AF_SUPPORTED)
    if(n3dParams.isDepthAFON) {
        _runDepthAF(n3dParams.magicNumber % DAF_TBL_QLEN, n3dParams.isAFTrigger);
    }

    if(n3dParams.isDistanceMeasurementON) {

    }
#endif

    char fileName[256];
    struct stat st;
    ::memset(&st, 0, sizeof(struct stat));
    FILE *fp = NULL;

#define N3D_UT_CASE_HAL_OUT_PATH "/sdcard/n3d"

#ifdef GTEST
    if(stat(N3D_UT_CASE_HAL_OUT_PATH, &st) == -1) {
        mkdir(N3D_UT_CASE_HAL_OUT_PATH, 0755);
    }

    fp = fopen(N3D_UT_CASE_HAL_OUT_PATH"/algo_output.txt", "w");
    if(fp) {
        m_pStereoDrv->StereoKernelFeatureCtrl( STEREO_KERNEL_FEATURE_DEBUG, fp, (void *)"XXX_DEBUG_INFO" );
        fflush(fp);
        fclose(fp);
        fp = NULL;
    }
#endif

    if( 1 == checkStereoProperty(PROPERTY_DUMP_OUTPUT) ) {
        sprintf(fileName, "%s/result_main1_%d.yuv", N3D_UT_CASE_HAL_OUT_PATH, n3dParams.magicNumber);
        MY_LOGD("Write %s...", fileName);
        const MSize OUTPUT_SIZE = StereoSizeProvider::getInstance()->getBufferSize(E_MV_Y);
        const MUINT32 OUTPUT_IMG_SIZE = OUTPUT_SIZE.w*OUTPUT_SIZE.h*1.5f;
        fp = fopen(fileName, "wb");
        if(NULL == fp) {
            MY_LOGE("Cannot open %s(err: %s)", fileName, ::strerror(errno));
        } else {
            fwrite(n3dOutput.rectifyImgMain1, 1, OUTPUT_IMG_SIZE, fp);
            fflush(fp);
            fclose(fp);
        }

        sprintf(fileName, "%s/result_main2_%d.yuv", N3D_UT_CASE_HAL_OUT_PATH, n3dParams.magicNumber);
        MY_LOGD("Write %s...", fileName);
        fp = fopen(fileName, "wb");
        if(NULL == fp) {
            MY_LOGE("Cannot open %s(err: %s)", fileName, ::strerror(errno));
        } else {
            fwrite(n3dOutput.rectifyImgMain2, 1, OUTPUT_IMG_SIZE, fp);
            fflush(fp);
            fclose(fp);
        }

        sprintf(fileName, "%s/mask1_%d.y", N3D_UT_CASE_HAL_OUT_PATH, n3dParams.magicNumber);
        MY_LOGD("Write %s...", fileName);
        fp = fopen(fileName, "wb");
        if(NULL == fp) {
            MY_LOGE("Cannot open %s(err: %s)", fileName, ::strerror(errno));
        } else {
            fwrite(n3dOutput.maskMain1, 1, sizeof(MUINT8)*OUTPUT_SIZE.w * OUTPUT_SIZE.h, fp);
            fflush(fp);
            fclose(fp);
        }

        sprintf(fileName, "%s/mask2_%d.y", N3D_UT_CASE_HAL_OUT_PATH, n3dParams.magicNumber);
        MY_LOGD("Write %s...", fileName);
        fp = fopen(fileName, "wb");
        if(NULL == fp) {
            MY_LOGE("Cannot open %s(err: %s)", fileName, ::strerror(errno));
        } else {
            fwrite(n3dOutput.maskMain2, 1, sizeof(MUINT8)*OUTPUT_SIZE.w * OUTPUT_SIZE.h, fp);
            fflush(fp);
            fclose(fp);
        }

        sprintf(fileName, "%s/ldc_%d.y", N3D_UT_CASE_HAL_OUT_PATH, n3dParams.magicNumber);
        MY_LOGD("Write %s...", fileName);
        fp = fopen(fileName, "wb");
        if(NULL == fp) {
            MY_LOGE("Cannot open %s(err: %s)", fileName, ::strerror(errno));
        } else {
            fwrite(n3dOutput.ldcMain1, 1, sizeof(MUINT8)*OUTPUT_SIZE.w * OUTPUT_SIZE.h, fp);
            fflush(fp);
            fclose(fp);
        }

    }

    return true;
}

bool
N3D_HAL_IMP::N3DHALRun(N3D_HAL_PARAM_CAPTURE &n3dParams, N3D_HAL_OUTPUT_CAPTURE &n3dOutput)
{
    _setN3DCaptureParams(n3dParams, n3dOutput);
    _runN3DCapture(n3dOutput);
    return true;
}

bool
N3D_HAL_IMP::N3DHALRun(N3D_HAL_PARAM_CAPTURE_SWFE &n3dParams, N3D_HAL_OUTPUT_CAPTURE &n3dOutput)
{
    _setN3DSWFECaptureParams(n3dParams, n3dOutput);
    _runN3DCapture(n3dOutput);
    return true;
}

char *
N3D_HAL_IMP::getStereoExtraData()
{
    //Only support capture
    if(eSTEREO_SCENARIO_CAPTURE != m_eScenario) {
        return NULL;
    }

    if(NULL == m_stereoExtraData) {
        _prepareStereoExtraData();
    }

    return m_stereoExtraData;
}
/**************************************************************************
 *       Private Functions                                                *
 **************************************************************************/
bool
N3D_HAL_IMP::_getFEOInputInfo(ENUM_PASS2_ROUND pass2Round,
                              ENUM_STEREO_SCENARIO eScenario,
                              STEREO_KERNEL_IMG_INFO_STRUCT &imgInfo)
{
    imgInfo.depth    = 1;    //pixel depth, YUV:1, RGB: 3, RGBA: 4
    imgInfo.format   = 0;    //YUV:0, RGB: 1

    Pass2SizeInfo pass2Info;
    StereoSizeProvider::getInstance()->getPass2SizeInfo(pass2Round, eScenario, pass2Info);

    imgInfo.width        = pass2Info.areaFEO.size.w;
    imgInfo.height       = pass2Info.areaFEO.size.h;
    imgInfo.stride       = imgInfo.width;
    imgInfo.act_width    = pass2Info.areaFEO.size.w - pass2Info.areaFEO.padding.w;
    imgInfo.act_height   = pass2Info.areaFEO.size.h - pass2Info.areaFEO.padding.h;

    if(pass2Round <= PASS2A_3) {
        imgInfo.offset_x = 0;
        imgInfo.offset_y = 0;
    } else {
        imgInfo.offset_x = (imgInfo.width - imgInfo.act_width)>>1;
        imgInfo.offset_y = (imgInfo.height - imgInfo.act_height)>>1;
    }

    return true;
}

bool
N3D_HAL_IMP::_initN3DGeoInfo(STEREO_KERNEL_GEO_INFO_STRUCT geo_img[])
{
//    if(MAX_GEO_LEVEL > 0) {
//        geo_img[0].size = StereoSettingProvider::fefmBlockSize(1);    //16
//        _getFEOInputInfo(PASS2A,        m_eScenario,    geo_img[0].img_main);
//        _getFEOInputInfo(PASS2A_P,      m_eScenario,    geo_img[0].img_auxi);
//    }

//    if(MAX_GEO_LEVEL > 1) {
        geo_img[0].size = StereoSettingProvider::fefmBlockSize(1);    //16
        _getFEOInputInfo(PASS2A_2,      m_eScenario,    geo_img[0].img_main);
        _getFEOInputInfo(PASS2A_P_2,    m_eScenario,    geo_img[0].img_auxi);
//    }

//    if(MAX_GEO_LEVEL > 2) {
        geo_img[1].size = StereoSettingProvider::fefmBlockSize(2);    //8
        _getFEOInputInfo(PASS2A_3,      m_eScenario,    geo_img[1].img_main);
        _getFEOInputInfo(PASS2A_P_3,    m_eScenario,    geo_img[1].img_auxi);
//    }

    return true;
}

bool
N3D_HAL_IMP::_initCCImgInfo(STEREO_KERNEL_IMG_INFO_STRUCT &ccImgInfo)
{
    Pass2SizeInfo pass2Info;
    StereoSizeProvider::getInstance()->getPass2SizeInfo(PASS2A_3, m_eScenario, pass2Info);
    MSize szCCImg = pass2Info.szIMG2O;
    //
    ccImgInfo.width         = szCCImg.w;
    ccImgInfo.height        = szCCImg.h;
    ccImgInfo.depth         = 1;            //pixel depth, YUV:1, RGB: 3, RGBA: 4
    ccImgInfo.stride        = szCCImg.w;
    ccImgInfo.format        = 4;//0;            //YUV:0, RGB: 1
    ccImgInfo.act_width     = szCCImg.w;
    ccImgInfo.act_height    = szCCImg.h;
    ccImgInfo.offset_x      = 0;
    ccImgInfo.offset_y      = 0;
    //
    return true;
}

MUINT8 *
N3D_HAL_IMP::_initWorkingBuffer(const MUINT32 BUFFER_SIZE)
{
    if (m_pWorkBuf) {
        delete [] m_pWorkBuf;
    }
    m_pWorkBuf = NULL;

    const MUINT32 BUFFER_LEN = sizeof(MUINT8) * BUFFER_SIZE;
    m_pWorkBuf = new(std::nothrow) MUINT8[BUFFER_LEN];
    if(NULL == m_pWorkBuf) {
        MY_LOGE("Cannot create N3D working buffer of size %d", BUFFER_LEN);
        return NULL;
    } else {
        MY_LOGD_IF(LOG_ENABLED, "Alloc %d bytes for N3D working buffer", BUFFER_LEN);
        ::memset(m_pWorkBuf, 0, BUFFER_LEN);
    }

    MINT32 err = 0; // 0: no error. other value: error.
    // Allocate working buffer.
    //     Allocate memory
    //     Set WorkBufInfo
    m_algoWorkBufInfo.ext_mem_size       = BUFFER_SIZE;
    m_algoWorkBufInfo.ext_mem_start_addr = m_pWorkBuf;

    err = m_pStereoDrv->StereoKernelFeatureCtrl(STEREO_KERNEL_FEATURE_SET_WORK_BUF_INFO,
                                                &m_algoWorkBufInfo, NULL);
    if (err)
    {
        MY_LOGE("StereoKernelFeatureCtrl(SET_WORK_BUF_INFO) fail. error code: %d.", err);
        return NULL;
    }

    return m_pWorkBuf;
}


void
N3D_HAL_IMP::_setN3DCommonParams(N3D_HAL_PARAM_COMMON &n3dParams, N3D_HAL_OUTPUT &n3dOutput, STEREO_KERNEL_SET_PROC_INFO_STRUCT &setprocInfo)
{
    setprocInfo.addr_ms  = n3dParams.rectifyImgMain1;
    setprocInfo.addr_md  = n3dOutput.rectifyImgMain1;
    setprocInfo.addr_mm  = n3dOutput.maskMain1;
    setprocInfo.addr_ml  = n3dOutput.ldcMain1;

    // for Photometric Correction
    setprocInfo.addr_mp   = n3dParams.ccImgMain1;
    setprocInfo.addr_ap   = n3dParams.ccImgMain2;
    // HWFE
    for(int i = 0; i < MAX_GEO_LEVEL; i++) {
        setprocInfo.addr_me[i] = n3dParams.hwfefmData.geoDataMain1[i];
        setprocInfo.addr_ae[i] = n3dParams.hwfefmData.geoDataMain2[i];
        setprocInfo.addr_lr[i] = n3dParams.hwfefmData.geoDataLeftToRight[i];
        setprocInfo.addr_rl[i] = n3dParams.hwfefmData.geoDataRightToLeft[i];
    }
    // AF INFO
    if(m_pAFTable) {
        const int AF_INDEX = n3dParams.magicNumber % DAF_TBL_QLEN;
        setprocInfo.dac_i = m_pAFTable->daf_vec[AF_INDEX].af_dac_index;
        setprocInfo.dac_v = m_pAFTable->daf_vec[AF_INDEX].af_valid;
        setprocInfo.dac_c = m_pAFTable->daf_vec[AF_INDEX].af_confidence;
        setprocInfo.dac_w[0] = m_pAFTable->daf_vec[AF_INDEX].af_win_start_x;
        setprocInfo.dac_w[1] = m_pAFTable->daf_vec[AF_INDEX].af_win_start_y;
        setprocInfo.dac_w[2] = m_pAFTable->daf_vec[AF_INDEX].af_win_end_x;
        setprocInfo.dac_w[3] = m_pAFTable->daf_vec[AF_INDEX].af_win_end_y;
    }
}

void
N3D_HAL_IMP::_setN3DParams(N3D_HAL_PARAM &n3dParams, N3D_HAL_OUTPUT &n3dOutput)
{
    _setN3DCommonParams(n3dParams, n3dOutput, m_algoProcInfo);
    m_algoProcInfo.addr_as = n3dParams.rectifyImgMain2;
    m_algoProcInfo.addr_ad = n3dOutput.rectifyImgMain2;
    m_algoProcInfo.addr_am = n3dOutput.maskMain2;

    // EIS INFO.
    if(n3dParams.eisData.isON) {
        m_algoProcInfo.eis[0] = n3dParams.eisData.eisOffset.x;
        m_algoProcInfo.eis[1] = n3dParams.eisData.eisOffset.y;
        m_algoProcInfo.eis[2] = n3dParams.eisData.eisImgSize.w;
        m_algoProcInfo.eis[3] = n3dParams.eisData.eisImgSize.h;
    } else {
        m_algoProcInfo.eis[0] = 0;
        m_algoProcInfo.eis[1] = 0;
        m_algoProcInfo.eis[2] = 0;
        m_algoProcInfo.eis[3] = 0;
    }

    _dumpSetProcInfo("", m_algoProcInfo);
    MINT32 err = m_pStereoDrv->StereoKernelFeatureCtrl(STEREO_KERNEL_FEATURE_SET_PROC_INFO, &m_algoProcInfo, NULL);
    if (err)
    {
        MY_LOGE("StereoKernelFeatureCtrl(SET_PROC) fail. error code: %d.", err);
        return;
    }

    _dumpSetProcInfo("", m_algoProcInfo);
}

void
N3D_HAL_IMP::_setN3DCaptureParams(N3D_HAL_PARAM_CAPTURE &n3dParams, N3D_HAL_OUTPUT_CAPTURE &n3dOutput)
{
    m_captureOrientation = n3dParams.captureOrientation;
    //
    _setN3DCommonParams(n3dParams, n3dOutput, m_algoProcInfo);
    //
    m_algoProcInfo.src_gb.mGraphicBuffer = (void *)&n3dParams.rectifyGBMain2;
    if(NULL == m_outputGBMain2.get()) {
        StereoArea imgArea = StereoSizeProvider::getInstance()->getBufferSize(E_SV_Y_LARGE);
        StereoDpUtil::allocImageBuffer(LOG_TAG, eImgFmt_RGBA8888, imgArea.size, IS_ALLOC_GB, m_outputGBMain2);
    }
    m_algoProcInfo.dst_gb.mGraphicBuffer = (void *)m_outputGBMain2.get();

    sp<GraphicBuffer>* srcGBArray[1];
    sp<GraphicBuffer>* dstGBArray[1];
    srcGBArray[0] = (sp<GraphicBuffer>*)&m_algoProcInfo.src_gb;
    dstGBArray[0] = (sp<GraphicBuffer>*)&m_algoProcInfo.dst_gb;
    m_algoProcInfo.InputGB   = (void*)&srcGBArray;
    m_algoProcInfo.OutputGB  = (void*)&dstGBArray;

    MINT32 err = m_pStereoDrv->StereoKernelFeatureCtrl(STEREO_KERNEL_FEATURE_SET_PROC_INFO, &m_algoProcInfo, NULL);
    if (err)
    {
        MY_LOGE("StereoKernelFeatureCtrl(SET_PROC) fail. error code: %d.", err);
        return;
    }

    _dumpSetProcInfo("", m_algoProcInfo);
}

bool
N3D_HAL_IMP::_runN3DCommon()
{
    bool bResult = true;

    MINT32 err = 0; // 0: no error. other value: error.

    MY_LOGD_IF(LOG_ENABLED, "StereoKernelMain +");
    err = m_pStereoDrv->StereoKernelMain();
    MY_LOGD_IF(LOG_ENABLED, "StereoKernelMain -");
    if (err) {
        MY_LOGE("StereoKernelMain() fail. error code: %d.", err);
        bResult = MFALSE;
    }

    // Get result.
    if(!err) {
        err = m_pStereoDrv->StereoKernelFeatureCtrl(STEREO_KERNEL_FEATURE_GET_RESULT, NULL, &m_algoResult);
        if (err)
        {
            MY_LOGE("StereoKernelFeatureCtrl(GET_RESULT) fail. error code: %d.", err);
            bResult = MFALSE;
        }
    }

    if(!err) {
        _dumpResult("[N3D Result]", m_algoResult);

        //Depth AF
        m_algoResult.out_n[0] ;
        m_algoResult.out_p[0] ;
        m_algoResult.out_n[1] ;
        m_algoResult.out_p[1] ;
    }

    return bResult;
}

bool
N3D_HAL_IMP::_runN3D(N3D_HAL_OUTPUT &n3dOutput __attribute__((unused)))
{
    bool bResult = true;
    bResult = _runN3DCommon();

    return bResult;
}

//Must run after getting result
bool
N3D_HAL_IMP::_runDepthAF(const int AF_INDEX, const bool IS_AF_TRIGGER)
{
    if(eSTEREO_SCENARIO_CAPTURE == m_eScenario ||
       NULL == m_pDepthAFHAL ||
       NULL == m_algoResult.out_p[1])
    {
        return false;
    }

    STEREODEPTH_HAL_INIT_PARAM_STRUCT stStereodepthHalInitParam;
    stStereodepthHalInitParam.pCoordTransParam = m_algoResult.out_p[1];
    m_pDepthAFHAL->StereoDepthSetParams(&stStereodepthHalInitParam);

    //TG -> screen caculation
    MFLOAT scl_w, scl_h, offset_x, offset_y ;
    scl_w = (MFLOAT) m_algoInitInfo.flow_main.sensor_size_w0/ m_algoInitInfo.flow_main.sensor_scale_w ;
    scl_h = (MFLOAT) m_algoInitInfo.flow_main.sensor_size_h0/ m_algoInitInfo.flow_main.sensor_scale_h ;
    offset_x = (m_algoInitInfo.flow_main.tg_offset_x + m_algoInitInfo.flow_main.sensor_offset_x1 ) * scl_w + m_algoInitInfo.flow_main.sensor_offset_x0 ;
    offset_y = (m_algoInitInfo.flow_main.tg_offset_y + m_algoInitInfo.flow_main.sensor_offset_y1 ) * scl_h + m_algoInitInfo.flow_main.sensor_offset_y0 ;

    //AF returns coordination in tg domain
    //Depth-AF requires screen domain
    //So we have to transfer af window coordination
    AF_WIN_COORDINATE_STRUCT stAfWinCoordinate;
    stAfWinCoordinate.af_win_start_x = (MUINT)( m_pAFTable->daf_vec[AF_INDEX].af_win_start_x * scl_w + offset_x );
    stAfWinCoordinate.af_win_start_y = (MUINT)( m_pAFTable->daf_vec[AF_INDEX].af_win_start_y * scl_h + offset_y );
    stAfWinCoordinate.af_win_end_x   = (MUINT)( m_pAFTable->daf_vec[AF_INDEX].af_win_end_x   * scl_w + offset_x );
    stAfWinCoordinate.af_win_end_y   = (MUINT)( m_pAFTable->daf_vec[AF_INDEX].af_win_end_y   * scl_h + offset_y );

    if(!IS_AF_TRIGGER) {
        MY_LOGD("[DAF][Learning] out_n[0] = %d, out_p[0] = %p", m_algoResult.out_n[0], m_algoResult.out_p[0]);
        if (0 != m_algoResult.out_n[0] &&
            NULL != m_algoResult.out_p[0])
        {
            m_pDepthAFHAL->StereoDepthRunLearning(m_algoResult.out_n[0], m_algoResult.out_p[0], &stAfWinCoordinate);
        }
    } else {
        MY_LOGD("[DAF][Query] out_n[0] = %d, out_p[0] = %p, AF win: (%d, %d), (%d, %d)",
                m_algoResult.out_n[0], m_algoResult.out_p[0],
                stAfWinCoordinate.af_win_start_x, stAfWinCoordinate.af_win_start_y,
                stAfWinCoordinate.af_win_end_x, stAfWinCoordinate.af_win_end_y);

        m_pDepthAFHAL->StereoDepthQuery(m_algoResult.out_n[0], m_algoResult.out_p[0], &stAfWinCoordinate);
    }

    return true;
}

bool
N3D_HAL_IMP::_runN3DCapture(N3D_HAL_OUTPUT_CAPTURE &n3dOutput)
{
    bool bResult = true;
    bResult = _runN3DCommon();

    if(eSTEREO_SCENARIO_CAPTURE != m_eScenario) {
        MY_LOGW("Wrong scenario, expect %d, fact: %d", eSTEREO_SCENARIO_CAPTURE, m_eScenario);
        return false;
    }

    if( 1 == checkStereoProperty(PROPERTY_ALGO_BEBUG) ) {
        static MUINT snLogCount = 0;
        m_pStereoDrv->StereoKernelFeatureCtrl(STEREO_KERNEL_FEATURE_SAVE_LOG, &snLogCount, NULL);
    }

    //Copy mask main1
    StereoArea areaMask = StereoSizeProvider::getInstance()->getBufferSize(E_MASK_M_Y, m_eScenario);
    MUINT32 length = areaMask.size.w * areaMask.size.h;
    memcpy(n3dOutput.maskMain1, m_main1Mask, length * sizeof(MUINT8));

    //=== Split mask ===
    struct timespec t_start, t_end, t_result;
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    //
    _splitMask();
    //
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    t_result = timeDiff(t_start, t_end);
    MY_LOGD_IF(BENCHMARK_ENABLED, "[Benchmark] Split mask: %lu.%.9lu", t_result.tv_sec, t_result.tv_nsec);

    //=== Transfer data to JSON ===
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    //
    _prepareStereoExtraData();
    //
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    t_result = timeDiff(t_start, t_end);
    MY_LOGD_IF(BENCHMARK_ENABLED, "[Benchmark] Encode extra data: %lu.%.9lu", t_result.tv_sec, t_result.tv_nsec);

    StereoArea imgArea = StereoSizeProvider::getInstance()->getBufferSize(E_SV_Y_LARGE);
    MUINT32 LENGTH = imgArea.size.w * imgArea.size.h;
    MUINT32 offset = 0;
    sp<IImageBuffer> srcImg;
    sp<IImageBuffer> dstImg;

    //Use MDP to centerize main1 image
    //1. Use MDP to resize and centerize main1 image, n3dOutput.rectifyImgMain1
    {
        // Create src image buffer
        StereoDpUtil::allocImageBuffer(LOG_TAG, eImgFmt_YV12, imgArea.size, !IS_ALLOC_GB, srcImg);
        // Cpoy data to dst image buffer
        MUINT32 offset = 0;
        ::memcpy((void*)srcImg.get()->getBufVA(0), n3dOutput.rectifyImgMain1, LENGTH);
        offset += LENGTH;
        ::memcpy((void*)srcImg.get()->getBufVA(1), n3dOutput.rectifyImgMain1+offset, LENGTH>>2);
        offset += (LENGTH>>2);
        ::memcpy((void*)srcImg.get()->getBufVA(2), n3dOutput.rectifyImgMain1+offset, LENGTH>>2);

        // Create dst image buffer
        StereoDpUtil::allocImageBuffer(LOG_TAG, eImgFmt_YV12, imgArea.size, !IS_ALLOC_GB, dstImg);
        ::memset((void*)dstImg.get()->getBufVA(0), 0, LENGTH);
        ::memset((void*)dstImg.get()->getBufVA(1), 0, LENGTH>>2);
        ::memset((void*)dstImg.get()->getBufVA(2), 0, LENGTH>>2);

        // rotate by MDP
//            DpRect *roi = new DpRect(imgArea.startPt.x, imgArea.startPt.y, imgArea.size.w, imgArea.size.h);  //no need to delete?
        DpRect *roi = new DpRect(0, 0, imgArea.size.w, imgArea.size.h);  //no need to delete?
        StereoDpUtil::transformImage(srcImg.get(), dstImg.get(), StereoSettingProvider::getModuleRotation(), roi);
        delete roi;

        // Copy rotated image from dst to n3dOutput.rectifyImgMain1
        offset = 0;
        ::memcpy(n3dOutput.rectifyImgMain1,         (MUINT8*)dstImg.get()->getBufVA(0), LENGTH);
        offset += LENGTH;
        ::memcpy(n3dOutput.rectifyImgMain1+offset,  (MUINT8*)dstImg.get()->getBufVA(1), LENGTH>>2);
        offset += (LENGTH>>2);
        ::memcpy(n3dOutput.rectifyImgMain1+offset,  (MUINT8*)dstImg.get()->getBufVA(2), LENGTH>>2);

        StereoDpUtil::freeImageBuffer(LOG_TAG, srcImg);
        StereoDpUtil::freeImageBuffer(LOG_TAG, dstImg);
    }

    //2. Use MDP to convert main2 image(m_outputGBMain2) from RGBA to YV12 for JPS, n3dOutput.jpsImgMain2
    {
        //Transform to YV12 by MDP
        StereoDpUtil::allocImageBuffer(LOG_TAG, eImgFmt_YV12, imgArea.size, !IS_ALLOC_GB, dstImg);
        StereoDpUtil::transformImage(m_outputGBMain2.get(), dstImg.get());

        //Copy data to n3dOutput.jpsImgMain2
        offset = 0;
        ::memcpy(n3dOutput.jpsImgMain2,         (MUINT8*)dstImg.get()->getBufVA(0), LENGTH);
        offset += LENGTH;
        ::memcpy(n3dOutput.jpsImgMain2+offset,  (MUINT8*)dstImg.get()->getBufVA(1), LENGTH>>2);
        offset += (LENGTH>>2);
        ::memcpy(n3dOutput.jpsImgMain2+offset,  (MUINT8*)dstImg.get()->getBufVA(2), LENGTH>>2);

        //3. Use MDP to resize main2 image, n3dOutput.rectifyImgMain2
        sp<IImageBuffer> resizedImg;
        MSize newSize = StereoSizeProvider::getInstance()->getBufferSize(E_SV_Y);
        if(StereoDpUtil::allocImageBuffer(LOG_TAG, eImgFmt_YV12, newSize, !IS_ALLOC_GB, resizedImg)) {
            StereoDpUtil::transformImage(dstImg.get(), resizedImg.get());

            //Copy data to n3dOutput.rectifyImgMain2
            const MUINT32 RESIZE_LEN = newSize.w * newSize.h;
            offset = 0;
            ::memcpy(n3dOutput.rectifyImgMain2,         (MUINT8*)resizedImg.get()->getBufVA(0), RESIZE_LEN);
            offset += RESIZE_LEN;
            ::memcpy(n3dOutput.rectifyImgMain2+offset,  (MUINT8*)resizedImg.get()->getBufVA(1), RESIZE_LEN>>2);
            offset += (RESIZE_LEN>>2);
            ::memcpy(n3dOutput.rectifyImgMain2+offset,  (MUINT8*)resizedImg.get()->getBufVA(2), RESIZE_LEN>>2);

            StereoDpUtil::freeImageBuffer(LOG_TAG, resizedImg);
        }

        StereoDpUtil::freeImageBuffer(LOG_TAG, dstImg);
    }

    //4. Use MDP to resize main2 mask and truccate, n3dOutput.maskMain2
    {
        sp<IImageBuffer> maskImg;
        if(StereoDpUtil::allocImageBuffer(LOG_TAG, eImgFmt_Y8, imgArea.size, !IS_ALLOC_GB, maskImg)) {
            //Copy mask to maskImg
            ::memcpy((MUINT8*)maskImg.get()->getBufVA(0), m_main2Mask, LENGTH);
        }

        sp<IImageBuffer> resizedMask;
        MSize newSize = StereoSizeProvider::getInstance()->getBufferSize(E_SV_Y);
        const MUINT32 RESIZE_LEN = newSize.w * newSize.h;
        if(StereoDpUtil::allocImageBuffer(LOG_TAG, eImgFmt_Y8, newSize, !IS_ALLOC_GB, resizedMask)) {
            StereoDpUtil::transformImage(maskImg.get(), resizedMask.get());

            //Copy data to n3dOutput.maskMain2
            const MUINT32 RESIZE_LEN = newSize.w * newSize.h;
            offset = 0;
            ::memcpy(n3dOutput.maskMain2,         (MUINT8*)resizedMask.get()->getBufVA(0), RESIZE_LEN);
            offset += RESIZE_LEN;
            ::memcpy(n3dOutput.maskMain2+offset,  (MUINT8*)resizedMask.get()->getBufVA(1), RESIZE_LEN>>2);
            offset += (RESIZE_LEN>>2);
            ::memcpy(n3dOutput.maskMain2+offset,  (MUINT8*)resizedMask.get()->getBufVA(2), RESIZE_LEN>>2);
        }

        //Truncate
        struct timespec t_start, t_end, t_result;
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        //
        for(MUINT32 i = 0; i < RESIZE_LEN; i++) {
            *(n3dOutput.maskMain2+i) &= 0XFF;
        }
        //
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        t_result = timeDiff(t_start, t_end);
        MY_LOGD_IF(BENCHMARK_ENABLED, "[Benchmark] Truncate mask: %lu.%.9lu", t_result.tv_sec, t_result.tv_nsec);

        StereoDpUtil::freeImageBuffer(LOG_TAG, maskImg);
        StereoDpUtil::freeImageBuffer(LOG_TAG, resizedMask);
    }

    return bResult;
}

void
N3D_HAL_IMP::_setN3DSWFECaptureParams(N3D_HAL_PARAM_CAPTURE_SWFE &n3dParams, N3D_HAL_OUTPUT_CAPTURE &n3dOutput)
{
    _setN3DCommonParams(n3dParams, n3dOutput, m_algoProcInfo);
//    MUINT8* addr_mg[MAX_GEO_LEVEL] ;  //SWFEFM_DATA.geo_src_image_main1
//    MUINT8* addr_ag[MAX_GEO_LEVEL] ;  //SWFEFM_DATA.geo_src_image_main2

    _dumpSetProcInfo("", m_algoProcInfo);
}

bool
N3D_HAL_IMP::_getStereoRemapInfo(STEREO_KERNEL_FLOW_INFO_STRUCT &infoMain1,
                                 STEREO_KERNEL_FLOW_INFO_STRUCT &infoMain2,
                                 ENUM_STEREO_SCENARIO eScenario)
{
    int sensorScenario = getSensorSenario(eScenario);

    MINT32 err = 0;
    int main1SensorIndex, main2SensorIndex;
    StereoSettingProvider::getStereoSensorIndex(main1SensorIndex, main2SensorIndex);

    int main1SensorDevIndex, main2SensorDevIndex;
    StereoSettingProvider::getStereoSensorDevIndex(main1SensorDevIndex, main2SensorDevIndex);

    IHalSensorList* sensorList = IHalSensorList::get();
    IHalSensor* pIHalSensor = NULL;
    SensorCropWinInfo rSensorCropInfo;
    ::memset(&rSensorCropInfo, 0, sizeof(SensorCropWinInfo));

    if(NULL == sensorList) {
        MY_LOGE("Cannot get sensor list");
    } else {
        MUINT32 junkStride;
        //========= Get main1 size =========
        IHalSensor* pIHalSensor = sensorList->createSensor(LOG_TAG, main1SensorIndex);
        if(NULL == pIHalSensor) {
            MY_LOGE("Cannot get hal sensor of main1");
            err = 1;
        } else {
            err = pIHalSensor->sendCommand(main1SensorDevIndex, SENSOR_CMD_GET_SENSOR_CROP_WIN_INFO,
                                           (MUINTPTR)&sensorScenario, (MUINTPTR)&rSensorCropInfo, 0);
        }

        if(!err) {
            infoMain1.pixel_array_width  = rSensorCropInfo.full_w;
            infoMain1.pixel_array_height = rSensorCropInfo.full_h ;
            infoMain1.sensor_offset_x0   = rSensorCropInfo.x0_offset ;
            infoMain1.sensor_offset_y0   = rSensorCropInfo.y0_offset ;
            infoMain1.sensor_size_w0     = rSensorCropInfo.w0_size ;
            infoMain1.sensor_size_h0     = rSensorCropInfo.h0_size ;
            infoMain1.sensor_scale_w     = rSensorCropInfo.scale_w ;
            infoMain1.sensor_scale_h     = rSensorCropInfo.scale_h ;
            infoMain1.sensor_offset_x1   = rSensorCropInfo.x1_offset ;
            infoMain1.sensor_offset_y1   = rSensorCropInfo.y1_offset ;
            infoMain1.tg_offset_x        = rSensorCropInfo.x2_tg_offset ;
            infoMain1.tg_offset_y        = rSensorCropInfo.y2_tg_offset ;

            infoMain1.rrz_usage_width    = rSensorCropInfo.w2_tg_size;      //sensor out width;
            infoMain1.rrz_usage_height   = (((rSensorCropInfo.w2_tg_size*9/16)>>1)<<1);

            MSize szMain1RRZO;
            MRect tgCropRect;
            StereoSizeProvider::getInstance()->getPass1Size( eSTEREO_SENSOR_MAIN1,
                                                             eImgFmt_FG_BAYER10,
                                                             EPortIndex_RRZO,
                                                             eScenario,
                                                             //below are outputs
                                                             tgCropRect,
                                                             szMain1RRZO,
                                                             junkStride);
            MINT32 uMaxRRZSize = (MUINT32)ceil(infoMain1.rrz_usage_width * RRZ_CAPIBILITY);
            if(uMaxRRZSize & 0x1) { uMaxRRZSize++ ; }   //rrz_out_width must be even number
            if(uMaxRRZSize > szMain1RRZO.w) {
                infoMain1.rrz_out_width  = uMaxRRZSize;
            } else {
                infoMain1.rrz_out_width  = szMain1RRZO.w;
            }

            //rrz_out_height must be an even number
            uMaxRRZSize = (MUINT32)ceil(infoMain1.rrz_usage_height * RRZ_CAPIBILITY);
            if(uMaxRRZSize & 0x1) { uMaxRRZSize++ ; }   //rrz_out_width must be even number
            if(uMaxRRZSize > szMain1RRZO.h) {
                infoMain1.rrz_out_height  = uMaxRRZSize;
            } else {
                infoMain1.rrz_out_height  = szMain1RRZO.h;
            }

            infoMain1.rrz_offset_x       = ((rSensorCropInfo.w2_tg_size - infoMain1.rrz_usage_width )>>1 ) ;
            infoMain1.rrz_offset_y       = ((rSensorCropInfo.h2_tg_size - infoMain1.rrz_usage_height)>>1 ) ;
        }

        if(pIHalSensor) {
            pIHalSensor->destroyInstance(LOG_TAG);
        }

        //========= Get main2 size =========
        pIHalSensor = sensorList->createSensor(LOG_TAG, main2SensorIndex);
        if(NULL == pIHalSensor) {
           MY_LOGE("Cannot get hal sensor of main2");
            err = 1;
        } else {
            err = pIHalSensor->sendCommand(main2SensorDevIndex, SENSOR_CMD_GET_SENSOR_CROP_WIN_INFO,
                                           (MUINTPTR)&sensorScenario, (MUINTPTR)&rSensorCropInfo, 0);
        }

        if(!err) {
            infoMain2.pixel_array_width  = rSensorCropInfo.full_w;
            infoMain2.pixel_array_height = rSensorCropInfo.full_h ;
            infoMain2.sensor_offset_x0   = rSensorCropInfo.x0_offset ;
            infoMain2.sensor_offset_y0   = rSensorCropInfo.y0_offset ;
            infoMain2.sensor_size_w0     = rSensorCropInfo.w0_size ;
            infoMain2.sensor_size_h0     = rSensorCropInfo.h0_size ;
            infoMain2.sensor_scale_w     = rSensorCropInfo.scale_w ;
            infoMain2.sensor_scale_h     = rSensorCropInfo.scale_h ;
            infoMain2.sensor_offset_x1   = rSensorCropInfo.x1_offset ;
            infoMain2.sensor_offset_y1   = rSensorCropInfo.y1_offset ;
            infoMain2.tg_offset_x        = rSensorCropInfo.x2_tg_offset ;
            infoMain2.tg_offset_y        = rSensorCropInfo.y2_tg_offset ;

            infoMain2.rrz_usage_width    = rSensorCropInfo.w2_tg_size;      //sensor out width;
            infoMain2.rrz_usage_height   = (((rSensorCropInfo.w2_tg_size*9/16)>>1)<<1);

            MRect tgCropRect;
            MSize szMain2RRZO;
            StereoSizeProvider::getInstance()->getPass1Size( eSTEREO_SENSOR_MAIN2,
                                                             eImgFmt_FG_BAYER10,
                                                             EPortIndex_RRZO,
                                                             eScenario,
                                                             //below are outputs
                                                             tgCropRect,
                                                             szMain2RRZO,
                                                             junkStride);
            MINT32 uMaxRRZSize = (MUINT32)ceil(infoMain2.rrz_usage_width * RRZ_CAPIBILITY);
            if(uMaxRRZSize & 0x1) { uMaxRRZSize++ ; }   //rrz_out_width must be even number
            if(uMaxRRZSize > szMain2RRZO.w) {
                infoMain2.rrz_out_width  = uMaxRRZSize;
            } else {
                infoMain2.rrz_out_width  = szMain2RRZO.w;
            }

            //rrz_out_height must be an even number
            uMaxRRZSize = (MUINT32)ceil(infoMain2.rrz_usage_height * RRZ_CAPIBILITY);
            if(uMaxRRZSize & 0x1) { uMaxRRZSize++ ; }   //rrz_out_width must be even number
            if(uMaxRRZSize > szMain2RRZO.h) {
                infoMain2.rrz_out_height  = uMaxRRZSize;
            } else {
                infoMain2.rrz_out_height  = szMain2RRZO.h;
            }

            infoMain2.rrz_offset_x       = ((rSensorCropInfo.w2_tg_size - infoMain2.rrz_usage_width )>>1 ) ;
            infoMain2.rrz_offset_y       = ((rSensorCropInfo.h2_tg_size - infoMain2.rrz_usage_height)>>1 ) ;
        }

        if(pIHalSensor) {
            pIHalSensor->destroyInstance(LOG_TAG);
        }
    }

    return true;
}

void
N3D_HAL_IMP::_splitMask()
{
    //====================================================================
    //  SPLITER: Split and rotate mask according to module orientation
    //  Result is stored in m_main2Mask(2176x1152)
    //====================================================================
    const int WIDTH  = m_algoInitInfo.img_out_wd;
    const int HEIGHT = m_algoInitInfo.img_out_ht;
    const int IMAGE_SIZE = WIDTH * HEIGHT;

    // init other memory for save rotate image.
    if(NULL == m_main2Mask) {
        m_main2Mask = new MUINT8[IMAGE_SIZE];
    }
    ::memset(m_main2Mask, 0, IMAGE_SIZE*sizeof(MUINT8));

    //Get mask from graphic buffer & rotate at the same time
    //Mask is 8-bit image, value: 0 or 0xFF

    //No rotation
    int nCol = 0;
    int nRow = 0;
    int nWritePos = 0;

//MODULE_ROTATE is defined in stereo_setting_provider.h
#if MODULE_ROTATE == 90
    //Rotate 90 degree clockwise
    nCol = HEIGHT - 1;
    nRow = 0;
    nWritePos = nCol;

#elif MODULE_ROTATE == 180
    //Rotate 180 degree
    nWritePos = IMAGE_SIZE - 1;

#elif MODULE_ROTATE == 270
    //Rotate 270 degree clockwise
    nCol = 0;
    nRow = WIDTH - 1;
    nWritePos = IMAGE_SIZE - HEIGHT;
#endif

    //Since graphic buffer is non-cacheable, which means direct access to gbuffer is very slow.
    //So we use cache to read and get mask
    //Performance enhancement is about 300ms -> 66 ms
    const MUINT32 CACHE_SIZE = 128; //experimental result, faster than 64, almost the same as 256, CACHE_SIZE % 8 = 0
    const MUINT32 COPY_SIZE = CACHE_SIZE * sizeof(MUINT32);

    MUINT32* pImgCache = new MUINT32[CACHE_SIZE];
    MUINT32* pGraphicBuffer = (MUINT32*)m_outputGBMain2.get();
    MUINT32* pCachePos = NULL;

    int nIndex = 0;
    for(int nRound = IMAGE_SIZE/CACHE_SIZE - 1; nRound >= 0; nRound--)
    {
        //Cache graphic buffer
        ::memcpy(pImgCache, pGraphicBuffer, COPY_SIZE);
        pGraphicBuffer += CACHE_SIZE;
        pCachePos = pImgCache;

        //Get mask from alpha channel and rotate at a time
        for(nIndex = CACHE_SIZE-1; nIndex >= 0; nIndex--) {
            *(m_main2Mask + nWritePos) = (MUINT8)((*pCachePos)>>24);
            ++pCachePos;

#if MODULE_ROTATE == 90
        //Rotate 90 degree clockwise
        nWritePos += HEIGHT;
        ++nRow;
        if(nRow >= WIDTH) {
            nRow = 0;
            --nCol;
            nWritePos = nCol;
        }

#elif MODULE_ROTATE == 180
        //Rotate 180 degree
        nWritePos--;

#elif MODULE_ROTATE == 270
        //Rotate 270 degree clockwise
        nWritePos -= HEIGHT;
        --nRow;
        if(nRow < 0) {
            nRow = WIDTH - 1;
            ++nCol;
            nWritePos = IMAGE_SIZE - HEIGHT + nCol;
        }
#else
        //No rotation
        ++nWritePos;
#endif
        }
    }
    delete [] pImgCache;
}

bool
N3D_HAL_IMP::N3DHALSaveNVRAM()
{
    Mutex::Autolock lock(mLock);
    m_nvramUserCount--;
    MY_LOGD("NVRAM User: %d", m_nvramUserCount);
    if(m_nvramUserCount > 0) {
        return true;
    } else if(m_nvramUserCount < 0) {
        MY_LOGW("Should not happen: NVRAM user %d", m_nvramUserCount);
        return false;
    }

    if(NULL == m_pVoidGeoData) {
        return false;
    }

    MBOOL bResult = MTRUE;
    MINT32 err = 0; // 0: no error. other value: error.

    err = m_pStereoDrv->StereoKernelFeatureCtrl(STEREO_KERNEL_FEATURE_SAVE_NVRAM,
                                                (void*)&m_pVoidGeoData->StereoNvramData.StereoData, NULL);
    if (err)
    {
        MY_LOGE("StereoKernelFeatureCtrl(STEREO_KERNEL_FEATURE_SAVE_NVRAM) fail. error code: %d.", err);
        return false;
    }

    int32_t main1DevIdx, main2DevIdx;
    StereoSettingProvider::getStereoSensorDevIndex(main1DevIdx, main2DevIdx);

    struct timespec t_start, t_end, t_result;
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    //
    err = NvBufUtil::getInstance().write(CAMERA_NVRAM_DATA_GEOMETRY, main1DevIdx);
    //
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    t_result = timeDiff(t_start, t_end);
    MY_LOGD_IF(BENCHMARK_ENABLED, "[Benchmark] Save NARAM time: %lu.%.9lu", t_result.tv_sec, t_result.tv_nsec);

    if (err) {
        MY_LOGE("Write to NVRAM fail.");
        bResult = MFALSE;
    } else {
        err = m_pStereoDrv->StereoKernelFeatureCtrl(STEREO_KERNEL_FEATURE_SAVE_NVRAM,
                                                    (void*)&m_pVoidGeoData->StereoNvramData.StereoData, NULL);
        MY_LOGD_IF(LOG_ENABLED, "Write to NVRAM success");
    }

    return bResult;
}

bool
N3D_HAL_IMP::_loadLensInfo()
{
    MRESULT err = m_pStereoDrv->StereoKernelFeatureCtrl(STEREO_KERNEL_FEATURE_LOAD_LENS_INFO, const_cast<float*>(LENS_INFO), NULL);
    if(err) {
        return false;
    }

    return true;
}

bool
N3D_HAL_IMP::_loadNVRAM()
{
    Mutex::Autolock lock(mLock);
    if(m_nvramUserCount < 0) {
        MY_LOGW("Should not happen: NVRAM user %d", m_nvramUserCount);
    }

    MY_LOGD_IF(LOG_ENABLED, "NVRAM User: %d", m_nvramUserCount);
    m_nvramUserCount++;
    if(m_pVoidGeoData) {
        return true;
    }

    int32_t main1DevIdx, main2DevIdx;
    StereoSettingProvider::getStereoSensorDevIndex(main1DevIdx, main2DevIdx);

    MINT32 err = NvBufUtil::getInstance().getBufAndRead(CAMERA_NVRAM_DATA_GEOMETRY, main1DevIdx, (void*&)m_pVoidGeoData);

    if(err) {
        MY_LOGE("Read NVRAM fail.");
        return false;
    }
#ifdef GTEST
    if(NULL == m_pVoidGeoData) {
        m_pVoidGeoData = new NVRAM_CAMERA_GEOMETRY_STRUCT;
        memset( &m_pVoidGeoData->StereoNvramData.StereoData, 0, sizeof(MINT32)*MTK_STEREO_KERNEL_NVRAM_LENGTH );
    }
    // === Prepare learning data ===
    const char *LEARN_FILE = "/data/nativetest/VSDoF_HAL_Test/N3D_UT/in/LearnINFO.n3d";
    MY_LOGD_IF(LOG_ENABLED, "Load Learning data from %s(len: %d)", LEARN_FILE, MTK_STEREO_KERNEL_NVRAM_LENGTH);
    FILE *fdata = fopen(LEARN_FILE, "r") ;
    if( fdata )
    {
        for( int i=0 ; i < MTK_STEREO_KERNEL_NVRAM_LENGTH ; i++ )
        {
            if(EOF == fscanf( fdata, "%d", &m_pVoidGeoData->StereoNvramData.StereoData[i] ) ) {
                break;
            }
        }

        fclose(fdata) ;
    } else {
        if(errno) {
            MY_LOGE("Cannot read learning data, error: %s", ::strerror(errno));
        } else {
            MY_LOGE("Cannot read learning data");
        }
    }
#endif

    if( 1 == checkStereoProperty(PROPERTY_NVRAM_BEBUG) ) {
        MY_LOGD_IF(LOG_ENABLED, "=== Dump loaded NVRAM ===");
        for(int i=0; i < MTK_STEREO_KERNEL_NVRAM_LENGTH; i++) {
            MY_LOGD_IF(LOG_ENABLED, "[%4d] %d. ", i, m_pVoidGeoData->StereoNvramData.StereoData[i]);
        }
    }

    err = m_pStereoDrv->StereoKernelFeatureCtrl(STEREO_KERNEL_FEATURE_LOAD_NVRAM,
                                                (void*)&m_pVoidGeoData->StereoNvramData.StereoData, NULL);
    if (err)
    {
        MY_LOGE("StereoKernelFeatureCtrl(STEREO_KERNEL_FEATURE_LOAD_NVRAM) fail. error code: %d.", err);
        return false;
    }

    return true;
}

void
N3D_HAL_IMP::_compressMask(std::vector<RUN_LENGTH_DATA> &compressedMask)
{
    compressedMask.clear();

    const int IMAGE_SIZE = m_algoInitInfo.img_out_wd * m_algoInitInfo.img_out_ht;
    MUINT32 len = 0;
    MUINT32 offset = 0;

    const int CMP_LEN = 128;
    MUINT8 *FF_MASK = new MUINT8[CMP_LEN];
    ::memset(FF_MASK, 0xFF, CMP_LEN);

    for(int i = 0; i < IMAGE_SIZE; i += CMP_LEN) {
        if(0 == memcmp(m_main2Mask, FF_MASK, CMP_LEN)) {
            if(0 == len) {
                offset = i;
            }

            len += CMP_LEN;
            m_main2Mask += CMP_LEN;
        } else {
            for(int j = 0; j < CMP_LEN; j++, m_main2Mask++) {
                if(0 != *m_main2Mask) {
                    if(0 != len) {
                        ++len;
                    } else {
                        len = 1;
                        offset = i+j;
                    }
                } else {
                    if(0 != len) {
                        compressedMask.push_back(RUN_LENGTH_DATA(offset, len));
                        len = 0;
                    }
                }
            }
        }
    }

    if(0 != len) {
        compressedMask.push_back(RUN_LENGTH_DATA(offset, len));
    }

    delete [] FF_MASK;
}

const char *
N3D_HAL_IMP::_prepareStereoExtraData()
{
    if(m_stereoExtraData) {
        delete m_stereoExtraData;
        m_stereoExtraData = NULL;
    }

//    "JPS_size": {
//        "width": 4352,
//        "height": 1152
//    },
    Document document;
    document.SetObject();
    Document::AllocatorType& allocator = document.GetAllocator();

    Value JPS_size(kObjectType);
    JPS_size.AddMember("width", m_algoInitInfo.img_src_wd*2, allocator);
    JPS_size.AddMember("height", m_algoInitInfo.img_src_ht, allocator);
    document.AddMember("JPS_size", JPS_size, allocator);

//    "output_image_size" : {
//        "width": 2176,
//        "height": 1152
//    },
    Value output_image_size(kObjectType);
    output_image_size.AddMember("width", m_algoInitInfo.img_out_wd, allocator);
    output_image_size.AddMember("height", m_algoInitInfo.img_out_ht, allocator);
    document.AddMember("output_image_size", output_image_size, allocator);

//    "main_cam_align_shift" : {
//        "x": 30,
//        "y": 10
//    },
    Value main_cam_align_shift(kObjectType);
    main_cam_align_shift.AddMember("x", m_algoResult.out_d[0], allocator);
    main_cam_align_shift.AddMember("y", m_algoResult.out_d[1], allocator);
    document.AddMember("main_cam_align_shift", main_cam_align_shift, allocator);

//    "input_image_size": {
//        "width": 1920,
//        "height": 1080
//    },
    Value input_image_size(kObjectType);
    input_image_size.AddMember("width", m_algoInitInfo.img_src_wd, allocator);
    input_image_size.AddMember("height", m_algoInitInfo.img_src_ht, allocator);
    document.AddMember("input_image_size", input_image_size, allocator);

//    "capture_orientation": {
//        "orientations_values": ["0: none", "1: flip_horizontal", "2: flip_vertical", "4: 90", "3: 180", "7: 270"],
//        "orientation": 0
//    },
    Value capture_orientation(kObjectType);
    Value orientations_values(kArrayType);
    orientations_values.PushBack(Value("0: none").Move(), allocator);
    orientations_values.PushBack(Value("1: flip_horizontal").Move(), allocator);
    orientations_values.PushBack(Value("2: flip_vertical").Move(), allocator);
    orientations_values.PushBack(Value("4: 90").Move(), allocator);
    orientations_values.PushBack(Value("3: 180").Move(), allocator);
    orientations_values.PushBack(Value("7: 270").Move(), allocator);
    capture_orientation.AddMember("orientations_values", orientations_values, allocator);
    capture_orientation.AddMember("orientation", Value(m_captureOrientation).Move(), allocator);
    document.AddMember("capture_orientation", capture_orientation, allocator);

//    "sensor_relative_position": {
//        "relative_position_values": ["0: main-minor", "1: minor-main"],
//        "relative_position": 0
//    },
    Value sensor_relative_position(kObjectType);
    Value relative_position_values(kArrayType);
    relative_position_values.PushBack(Value("0: main-minor").Move(), allocator);
    relative_position_values.PushBack(Value("1: minor-main").Move(), allocator);
    sensor_relative_position.AddMember("relative_position_values", relative_position_values, allocator);
    sensor_relative_position.AddMember("relative_position", Value(StereoSettingProvider::getSensorRelativePosition()).Move(), allocator);
    document.AddMember("sensor_relative_position", sensor_relative_position, allocator);

//    "focus_roi": {
//        "top": 0,
//        "left": 10,
//        "bottom": 10,
//        "right": 30
//    },
    Value focus_roi(kObjectType);
    focus_roi.AddMember("top",      Value(m_afROI.leftTop().y).Move(), allocator);
    focus_roi.AddMember("left",     Value(m_afROI.leftTop().x).Move(), allocator);
    focus_roi.AddMember("bottom",   Value(m_afROI.rightBottom().y).Move(), allocator);
    focus_roi.AddMember("right",    Value(m_afROI.rightBottom().x).Move(), allocator);
    document.AddMember("focus_roi", focus_roi, allocator);

//    "verify_geo_data": {
//        "quality_level_values": ["0: PASS","1: WARN","2: FAIL"],
//        "quality_level": 0,
//        "statistics": [0,0,0,0,0,0]
//    },
    Value verify_geo_data(kObjectType);
    Value quality_level_values(kArrayType);
    quality_level_values.PushBack(Value("0: PASS").Move(), allocator);
    quality_level_values.PushBack(Value("1: WARN").Move(), allocator);
    quality_level_values.PushBack(Value("2: FAIL").Move(), allocator);
    verify_geo_data.AddMember("quality_level_values", quality_level_values, allocator);
    verify_geo_data.AddMember("quality_level", Value(m_algoResult.out_v[0]).Move(), allocator);
    Value geo_statistics(kArrayType);
    for(int i = 0; i < 6; i++) {
        geo_statistics.PushBack(Value(m_algoResult.out_v[i+2]).Move(), allocator);
    }
    verify_geo_data.AddMember("statistics", geo_statistics, allocator);
    document.AddMember("verify_geo_data", verify_geo_data, allocator);

//    "verify_pho_data": {
//        "quality_level_values": ["0: PASS","1: WARN","2: FAIL"],
//        "quality_level": 0,
//        "statistics": [0,0,0,0]
//    },
    Value verify_pho_data(kObjectType);
    Value pho_quality_level_values(kArrayType);
    pho_quality_level_values.PushBack(Value("0: PASS").Move(), allocator);
    pho_quality_level_values.PushBack(Value("1: WARN").Move(), allocator);
    pho_quality_level_values.PushBack(Value("2: FAIL").Move(), allocator);
    verify_pho_data.AddMember("quality_level_values", pho_quality_level_values, allocator);
    verify_pho_data.AddMember("quality_level", Value(m_algoResult.out_v[1]).Move(), allocator);
    Value pho_statistics(kArrayType);
    for(int i = 0; i < 4; i++) {
        pho_statistics.PushBack(Value(m_algoResult.out_v[i+8]).Move(), allocator);
    }
    verify_pho_data.AddMember("statistics", pho_statistics, allocator);
    document.AddMember("verify_pho_data", verify_pho_data, allocator);

//    "verify_mtk_cha": {
//        "check_values": ["0: PASS","1: FAIL"],
//        "check": 0,
//        "score": 0
//    },
    Value verify_mtk_cha(kObjectType);
    Value verify_mtk_cha_values(kArrayType);
    verify_mtk_cha_values.PushBack(Value("0: PASS").Move(), allocator);
    verify_mtk_cha_values.PushBack(Value("1: FAIL").Move(), allocator);
    verify_mtk_cha.AddMember("check_values", verify_mtk_cha_values, allocator);
    verify_mtk_cha.AddMember("check", Value(m_algoResult.out_v[12]).Move(), allocator);
    verify_mtk_cha.AddMember("score", Value(m_algoResult.out_v[13]).Move(), allocator);
    document.AddMember("verify_mtk_cha", verify_mtk_cha, allocator);

//    "depth_buffer_size": {
//        "width": 480,
//        "height": 270
//    },
    Value depth_buffer_size(kObjectType);
    MSize szDepthMap = StereoSizeProvider::getInstance()->getBufferSize(E_DEPTH_MAP);
    depth_buffer_size.AddMember("width", szDepthMap.w, allocator);
    depth_buffer_size.AddMember("height", szDepthMap.h, allocator);
    document.AddMember("depth_buffer_size", depth_buffer_size, allocator);

//    "mask_info" : {
//        "width":2176,
//        "height":1152,
//        "mask description": "Data(0xFF), format: [offset,length]",
//        "mask": [[28,1296],[1372,1296],[2716,1296],...]
//    }
    Value mask_info(kObjectType);
    mask_info.AddMember("width", m_algoInitInfo.img_out_wd, allocator);
    mask_info.AddMember("height", m_algoInitInfo.img_out_ht, allocator);
    mask_info.AddMember("mask description", "Data(0xFF), format: [offset,length]", allocator);

    struct timespec t_start, t_end, t_result;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    std::vector<RUN_LENGTH_DATA> runLengthMaskData;
    _compressMask(runLengthMaskData);

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    t_result = timeDiff(t_start, t_end);
    MY_LOGD_IF(BENCHMARK_ENABLED, "[Benchmark] Compress mask: %lu.%.9lu(len:%zu)", t_result.tv_sec, t_result.tv_nsec, runLengthMaskData.size());

    Value mask(kArrayType);
    for(std::vector<RUN_LENGTH_DATA>::iterator it = runLengthMaskData.begin(); it != runLengthMaskData.end(); ++it) {
        Value maskData(kArrayType);
        maskData.PushBack(Value(it->offset).Move(), allocator);
        maskData.PushBack(Value(it->len).Move(), allocator);
        mask.PushBack(maskData.Move(), allocator);
    }
    mask_info.AddMember("mask", mask, allocator);
    document.AddMember("mask_info", mask_info, allocator);

    StringBuffer sb;
    Writer<StringBuffer> writer(sb);
    document.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
//    MY_LOGD_IF(LOG_ENABLED, "JSON: %s:", sb.GetString());

//    FILE* fp = fopen("/sdcard/dbg2.json", "wb"); // non-Windows use "w"
//    char writeBuffer[1024];
//    FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
//    Writer<FileWriteStream> writer(os);
//    document.Accept(writer);
//    fclose(fp);

    const char *stereoExtraData = sb.GetString();
    if(stereoExtraData) {
        const int STR_LEN = strlen(stereoExtraData);
        if(STR_LEN > 0) {
            m_stereoExtraData = new char[STR_LEN+1];
            strcpy(m_stereoExtraData, stereoExtraData);
        }
    }

    return m_stereoExtraData;
}

// Logger
void
N3D_HAL_IMP::_dumpInitInfo(const STEREO_KERNEL_SET_ENV_INFO_STRUCT &initInfo)
{
    if(!LOG_ENABLED) {
        return;
    }

    MY_LOGD("========= N3D Init Info =========");
    MY_LOGD("[scenario]     %d", initInfo.scenario);

    // ALGORITHM INPUT and SbS OUTPUT
    MY_LOGD("[img_src_wd]   %d", initInfo.img_src_wd);
    MY_LOGD("[img_src_ht]   %d", initInfo.img_src_ht);
    MY_LOGD("[img_out_wd]   %d", initInfo.img_out_wd);
    MY_LOGD("[img_out_ht]   %d", initInfo.img_out_ht);
    MY_LOGD("[img_inp_wd]   %d", initInfo.img_inp_wd);
    MY_LOGD("[img_inp_ht]   %d", initInfo.img_inp_ht);

    // HWFE INPUT - the actual size for HWFE (after SRZ)
    MY_LOGD("[geo_info]  %d", initInfo.geo_info);

    char logPrefix[32];
    int i = 0;
    for(i = 0; i < MAX_GEO_LEVEL; i++) {
        MY_LOGD("[geo_img][%d][size] %d", i, initInfo.geo_img[i].size);

        sprintf(logPrefix, "[geo_img][%d][main]", i);
        _dumpImgInfo(logPrefix, initInfo.geo_img[i].img_main);

        sprintf(logPrefix, "[geo_img][%d][auxi]", i);
        _dumpImgInfo(logPrefix, initInfo.geo_img[i].img_auxi);
    }

    // COLOR CORRECTION INPUT
    _dumpImgInfo("[pho_img]", initInfo.pho_img);

    // Learning
    MY_LOGD("[hfov_main]    %.1f", initInfo.hfov_main);
    MY_LOGD("[hfov_auxi]    %.1f", initInfo.hfov_auxi);
    MY_LOGD("[baseline]     %.1f", initInfo.baseline);
    std::string s = std::bitset<8>(initInfo.system_cfg).to_string();
    MY_LOGD("[system_cfg]   %s", s.c_str());
    MY_LOGD("[dac_start]    %d", initInfo.dac_start);

    // Learning Coordinates RE-MAPPING
    _dumpRemapInfo("[flow_main]", initInfo.flow_main);
    _dumpRemapInfo("[flow_auxi]", initInfo.flow_auxi);

    // OUTPUT after Initialization
    MUINT32 working_buffer_size ;
    MY_LOGD("[working_buffer_size]  %d", initInfo.working_buffer_size);
    _dumpTuningInfo("", *(initInfo.ptuning_para));
}

void
N3D_HAL_IMP::_dumpImgInfo(const char *prefix, const STEREO_KERNEL_IMG_INFO_STRUCT &imgInfo)
{
    if(!LOG_ENABLED) {
        return;
    }

    MY_LOGD("%s[width]      %d", prefix, imgInfo.width);
    MY_LOGD("%s[height]     %d", prefix, imgInfo.height);
    MY_LOGD("%s[depth]      %d", prefix, imgInfo.depth);
    MY_LOGD("%s[stride]     %d", prefix, imgInfo.stride);
    MY_LOGD("%s[format]     %d", prefix, imgInfo.format);
    MY_LOGD("%s[act_width]  %d", prefix, imgInfo.act_width);
    MY_LOGD("%s[act_height] %d", prefix, imgInfo.act_height);
    MY_LOGD("%s[offset_x]   %d", prefix, imgInfo.offset_x);
    MY_LOGD("%s[offset_y]   %d", prefix, imgInfo.offset_y);
}

void
N3D_HAL_IMP::_dumpRemapInfo(const char *prefix, const STEREO_KERNEL_FLOW_INFO_STRUCT &remapInfo)
{
    if(!LOG_ENABLED) {
        return;
    }

    MY_LOGD("%s[pixel_array_width]    %d", prefix, remapInfo.pixel_array_width);
    MY_LOGD("%s[pixel_array_height]   %d", prefix, remapInfo.pixel_array_height);
    MY_LOGD("%s[sensor_offset_x0]     %d", prefix, remapInfo.sensor_offset_x0);
    MY_LOGD("%s[sensor_offset_y0]     %d", prefix, remapInfo.sensor_offset_y0);
    MY_LOGD("%s[sensor_size_w0]       %d", prefix, remapInfo.sensor_size_w0);
    MY_LOGD("%s[sensor_size_h0]       %d", prefix, remapInfo.sensor_size_h0);
    MY_LOGD("%s[sensor_scale_w]       %d", prefix, remapInfo.sensor_scale_w);
    MY_LOGD("%s[sensor_scale_h]       %d", prefix, remapInfo.sensor_scale_h);
    MY_LOGD("%s[sensor_offset_x1]     %d", prefix, remapInfo.sensor_offset_x1);
    MY_LOGD("%s[sensor_offset_y1]     %d", prefix, remapInfo.sensor_offset_y1);
    MY_LOGD("%s[tg_offset_x]          %d", prefix, remapInfo.tg_offset_x);
    MY_LOGD("%s[tg_offset_y]          %d", prefix, remapInfo.tg_offset_y);
    MY_LOGD("%s[rrz_offset_x]         %d", prefix, remapInfo.rrz_offset_x);
    MY_LOGD("%s[rrz_offset_y]         %d", prefix, remapInfo.rrz_offset_y);
    MY_LOGD("%s[rrz_usage_width]      %d", prefix, remapInfo.rrz_usage_width);
    MY_LOGD("%s[rrz_usage_height]     %d", prefix, remapInfo.rrz_usage_height);
    MY_LOGD("%s[rrz_out_width]        %d", prefix, remapInfo.rrz_out_width);
    MY_LOGD("%s[rrz_out_height]       %d", prefix, remapInfo.rrz_out_height);
}

void
N3D_HAL_IMP::_dumpTuningInfo(const char *prefix, const STEREO_KERNEL_TUNING_PARA_STRUCT &tuningInfo)
{
    if(!LOG_ENABLED) {
        return;
    }

    std::string s = std::bitset<32>(tuningInfo.p_tune).to_string();
    MY_LOGD("%s[p_tune]   %s", prefix, s.c_str());
    s = std::bitset<32>(tuningInfo.s_tune).to_string();
    MY_LOGD("%s[s_tune]   %s", prefix, s.c_str());
}

void
N3D_HAL_IMP::_dumpSetProcInfo(const char *prefix, const STEREO_KERNEL_SET_PROC_INFO_STRUCT &setprocInfo)
{
    if(!LOG_ENABLED) {
        return;
    }

    MY_LOGD("%s[addr_ms]    %p", prefix, setprocInfo.addr_ms);
    MY_LOGD("%s[addr_md]    %p", prefix, setprocInfo.addr_md);
    MY_LOGD("%s[addr_mm]    %p", prefix, setprocInfo.addr_mm);
    MY_LOGD("%s[addr_ml]    %p", prefix, setprocInfo.addr_ml);

    MY_LOGD("%s[addr_as]    %p", prefix, setprocInfo.addr_as);
    MY_LOGD("%s[addr_ad]    %p", prefix, setprocInfo.addr_ad);
    MY_LOGD("%s[addr_am]    %p", prefix, setprocInfo.addr_am);

    MY_LOGD("%s[addr_mp]    %p", prefix, setprocInfo.addr_mp);
    MY_LOGD("%s[addr_ap]    %p", prefix, setprocInfo.addr_ap);

    MY_LOGD("%s[addr_mg]    %p %p %p", prefix, setprocInfo.addr_mg[0], setprocInfo.addr_mg[1], setprocInfo.addr_mg[2]);
    MY_LOGD("%s[addr_ag]    %p %p %p", prefix, setprocInfo.addr_ag[0], setprocInfo.addr_ag[1], setprocInfo.addr_ag[2]);

    MY_LOGD("%s[src_gb]     mGraphicBuffer %p, mEGLImage %p", prefix, setprocInfo.src_gb.mGraphicBuffer, setprocInfo.src_gb.mEGLImage);
    MY_LOGD("%s[dst_gb]     mGraphicBuffer %p, mEGLImage %p", prefix, setprocInfo.dst_gb.mGraphicBuffer, setprocInfo.dst_gb.mEGLImage);

    MY_LOGD("%s[addr_me]    %p %p %p", prefix, setprocInfo.addr_me[0], setprocInfo.addr_me[1], setprocInfo.addr_me[2]);
    MY_LOGD("%s[addr_ae]    %p %p %p", prefix, setprocInfo.addr_ae[0], setprocInfo.addr_ae[1], setprocInfo.addr_ae[2]);
    MY_LOGD("%s[addr_rl]    %p %p %p", prefix, setprocInfo.addr_rl[0], setprocInfo.addr_rl[1], setprocInfo.addr_rl[2]);
    MY_LOGD("%s[addr_lr]    %p %p %p", prefix, setprocInfo.addr_lr[0], setprocInfo.addr_lr[1], setprocInfo.addr_lr[2]);

    MY_LOGD("%s[dac_i]      %d", prefix, setprocInfo.dac_i);
    MY_LOGD("%s[dac_v]      %d", prefix, setprocInfo.dac_v);
    MY_LOGD("%s[dac_c]      %.5f", prefix, setprocInfo.dac_c);
    MY_LOGD("%s[dac_w]      %d %d %d %d", prefix, setprocInfo.dac_w[0], setprocInfo.dac_w[1], setprocInfo.dac_w[2], setprocInfo.dac_w[3]);
    MY_LOGD("%s[eis]        %d %d %d %d", prefix, setprocInfo.eis[0], setprocInfo.eis[1], setprocInfo.eis[2], setprocInfo.eis[3]);

    MY_LOGD("%s[eglDisplay] %p", prefix, setprocInfo.eglDisplay);
    MY_LOGD("%s[InputGB]    %p", prefix, setprocInfo.InputGB);
    MY_LOGD("%s[OutputGB]   %p", prefix, setprocInfo.OutputGB);
}

void
N3D_HAL_IMP::_dumpResult(const char *prefix, const STEREO_KERNEL_RESULT_STRUCT &n3dResult)
{
    if(!LOG_ENABLED) {
        return;
    }

    MY_LOGD("%s[out_d]       %.5f %.5f %.5f %.5f", prefix, n3dResult.out_d[0], n3dResult.out_d[1], n3dResult.out_d[2], n3dResult.out_d[3]);
    MY_LOGD("%s[out_n]       %10d %10d %10d", prefix, n3dResult.out_n[0], n3dResult.out_n[1], n3dResult.out_n[2]);
    MY_LOGD("%s[out_p]       %p %p %p", prefix, n3dResult.out_p[0], n3dResult.out_p[1], n3dResult.out_p[2]);
    MY_LOGD("%s[out_v[0]]    %d", prefix, n3dResult.out_v[0]);
    MY_LOGD("%s[out_v[1]]    %d", prefix, n3dResult.out_v[1]);
    std::string s = std::bitset<4>(n3dResult.out_v[2]).to_string();
    MY_LOGD("%s[out_v[2]]    %s", prefix, s.c_str());
    for(int i = 3; i < 14; i++) {
        MY_LOGD("%s[out_v[%d]]    %d", prefix, i, n3dResult.out_v[i]);
    }
}
