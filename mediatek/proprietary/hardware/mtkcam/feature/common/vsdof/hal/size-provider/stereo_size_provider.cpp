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
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#define LOG_TAG "StereoSizeProvider"

#include <math.h>
//
#include <metadata/IMetadataProvider.h>
#include <metadata/client/mtk_metadata_tag.h>
//
#include <vsdof/hal/stereo_setting_provider.h>
#include <vsdof/hal/stereo_size_provider.h>
#include "pass2/pass2A_size_providers.h"

using android::Mutex;
#define LOG_TAG "StereoSizeProvider"

#define STEREO_SIZE_PROVIDER_DEBUG

#ifdef STEREO_SIZE_PROVIDER_DEBUG    // Enable debug log.

#undef __func__
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
#endif  // STEREO_SIZE_PROVIDER_DEBUG

//===============================================================
//  Singleton and init operations
//===============================================================
StereoSizeProvider *StereoSizeProvider::_instance = NULL;
Mutex StereoSizeProvider::mLock;
volatile MINT32 StereoSizeProvider::mUsers;

StereoSizeProvider *
StereoSizeProvider::getInstance()
{
    Mutex::Autolock lock(mLock);
    if(NULL == _instance) {
        _instance = new StereoSizeProvider();
        mUsers = 0;
    }

    android_atomic_inc(&mUsers);
    return _instance;
}

void
StereoSizeProvider::destroyInstance()
{
    Mutex::Autolock lock(mLock);

    if(mUsers > 0) {
        android_atomic_dec(&mUsers);
    } else if(NULL != _instance) {
        delete _instance;
        _instance = NULL;
    }
}

StereoSizeProvider::~StereoSizeProvider()
{
    if(NULL != _instance) {
        delete _instance;
        _instance = NULL;
    }
}

bool
StereoSizeProvider::getPass1Size( ENUM_STEREO_SENSOR sensor,
                                  EImageFormat format,
                                  EPortIndex port,
                                  ENUM_STEREO_SCENARIO scenario,
                                  MRect &tgCropRect,
                                  MSize &outSize,
                                  MUINT32 &strideInBytes
                                ) const
{
    // Get sensor senario
    int sensorScenario = getSensorSenario(scenario);

    // Prepare sensor hal
    IHalSensorList* sensorList = IHalSensorList::get();
    if(NULL == sensorList) {
        MY_LOGE("Cannot get sensor list");
        return false;
    }

    MINT32 err = 0;
    int32_t main1Idx, main2Idx;
    StereoSettingProvider::getStereoSensorIndex(main1Idx, main2Idx);
    int sendorDevIndex = sensorList->querySensorDevIdx((eSTEREO_SENSOR_MAIN1 == sensor) ? main1Idx : main2Idx);
    IHalSensor* pIHalSensor = sensorList->createSensor(LOG_TAG, (eSTEREO_SENSOR_MAIN1 == sensor) ? main1Idx : main2Idx);
    if(NULL == pIHalSensor) {
        MY_LOGE("Cannot get hal sensor");
        return false;
    }

    SensorStaticInfo sensorStaticInfo;
    memset(&sensorStaticInfo, 0, sizeof(SensorStaticInfo));
    sensorList->querySensorStaticInfo(sendorDevIndex, &sensorStaticInfo);

    StereoArea result;
    tgCropRect.p.x = 0;
    tgCropRect.p.y = 0;
    tgCropRect.s.w = (scenario == eSTEREO_SCENARIO_CAPTURE) ? sensorStaticInfo.captureWidth  : sensorStaticInfo.previewWidth;
    tgCropRect.s.h = (scenario == eSTEREO_SCENARIO_CAPTURE) ? sensorStaticInfo.captureHeight : sensorStaticInfo.previewHeight;
    outSize.w = tgCropRect.s.w;
    outSize.h = tgCropRect.s.h;

    if(EPortIndex_RRZO == port) {
        outSize.w = sensorStaticInfo.previewWidth;
        outSize.h = ((outSize.w * 9 / 16)>>1) << 1;

        int height = tgCropRect.s.h;
        tgCropRect.s.h = ((tgCropRect.s.w * 9 / 16)>>1) << 1;
        tgCropRect.p.x = 0;
        tgCropRect.p.y = (height - tgCropRect.s.h)/2;
    }

    //Get FPS
    int defaultFPS = 0; //result will be 10xFPS, e.g. if 30 fps, defaultFPS = 300
    err = pIHalSensor->sendCommand(sendorDevIndex, SENSOR_CMD_GET_DEFAULT_FRAME_RATE_BY_SCENARIO,
                                   (MINTPTR)&sensorScenario, (MINTPTR)&defaultFPS, 0);
    if(err) {
        MY_LOGE("Cannot get default frame rate");
        return false;
    }

    //Get pixel format
    E_ISP_PIXMODE pixelMode;
    defaultFPS /= 10;
    err = pIHalSensor->sendCommand(sendorDevIndex, SENSOR_CMD_GET_SENSOR_PIXELMODE,
                                   (MINTPTR)&sensorScenario, (MINTPTR)&defaultFPS, (MINTPTR)&pixelMode);
    if(err) {
        MY_LOGE("Cannot get pixel mode");
        return false;
    }

    NSImageio::NSIspio::ISP_QUERY_RST queryRst;
    NSImageio::NSIspio::ISP_QuerySize( port,
                                       NSImageio::NSIspio::ISP_QUERY_X_PIX
                                       | NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX
                                       | NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
                                       format,
                                       outSize.w,
                                       queryRst,
                                       pixelMode
                                     );

    strideInBytes = queryRst.stride_byte;

    tgCropRect.p.x = (outSize.w - queryRst.x_pix)/2 * tgCropRect.s.w / outSize.w;
    outSize.w = queryRst.x_pix;

    pIHalSensor->destroyInstance(LOG_TAG);
    return true;
}

template <typename T>
inline MBOOL
tryGetMetadata(
    IMetadata const* pMetadata,
    MUINT32 const tag,
    T & rVal
)
{
    if (pMetadata == NULL) {
        MY_LOGW("pMetadata == NULL");
        return MFALSE;
    }
    //
    IMetadata::IEntry entry = pMetadata->entryFor(tag);
    if(!entry.isEmpty()) {
        rVal = entry.itemAt(0, Type2Type<T>());
        return MTRUE;
    }
    //
    return MFALSE;
}

bool
StereoSizeProvider::getPass1ActiveArrayCrop(ENUM_STEREO_SENSOR sensor, MRect &cropRect)
{
    int main1Id, main2Id;
    StereoSettingProvider::getStereoSensorIndex(main1Id, main2Id);
    int sensorId = -1;
    switch(sensor)
    {
        case eSTEREO_SENSOR_MAIN1:
            sensorId = main1Id;
            break;
        case eSTEREO_SENSOR_MAIN2:
            sensorId = main2Id;
            break;
        default:
            break;
    }

    if(sensorId < 0) {
        MY_LOGW("Wrong sensor: %d", sensorId);
        return false;
    }

    sp<IMetadataProvider> pMetadataProvider = NSMetadataProviderManager::valueFor(sensorId);
    if( ! pMetadataProvider.get() ) {
        MY_LOGE("MetadataProvider is NULL");
        return false;
    }

    bool result = false;
    IMetadata static_meta = pMetadataProvider->geMtktStaticCharacteristics();
    if( tryGetMetadata<MRect>(&static_meta, MTK_SENSOR_INFO_ACTIVE_ARRAY_REGION, cropRect) ) {
        cropRect.s.w = (cropRect.s.w>>1)<<1;
        int newHeight = ((cropRect.s.w * 9 / 16)>>1)<<1;
        cropRect.p.y = (cropRect.s.h-newHeight)/2;
        cropRect.s.h = newHeight;
        result = true;
    }

    return result;
}

bool
StereoSizeProvider::getPass2SizeInfo(ENUM_PASS2_ROUND round, ENUM_STEREO_SCENARIO eScenario, Pass2SizeInfo &pass2SizeInfo)
{
    bool isSuccess = true;
    switch(round) {
        case PASS2A:
            pass2SizeInfo = Pass2A_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_2:
            pass2SizeInfo = Pass2A_2_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_3:
            pass2SizeInfo = Pass2A_3_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_P:
            pass2SizeInfo = Pass2A_P_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_P_2:
            pass2SizeInfo = Pass2A_P_2_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        case PASS2A_P_3:
            pass2SizeInfo = Pass2A_P_3_SizeProvider::instance()->sizeInfo(eScenario);
            break;
        default:
            isSuccess = false;
    }

    return isSuccess;
}

StereoArea
StereoSizeProvider::getBufferSize(ENUM_BUFFER_NAME eName, ENUM_STEREO_SCENARIO eScenario) const
{
    switch(eName) {
        //N3D before MDP for capture
        case E_MV_Y_LARGE:
        case E_MASK_M_Y_LARGE:
        case E_SV_Y_LARGE:
        case E_MASK_S_Y_LARGE:
            if(eSTEREO_SCENARIO_CAPTURE == eScenario) {
                return StereoArea(2176, 1152, 256, 72, 128, 36).rotatedByModule();
            } else {
                return STEREO_AREA_ZERO;
            }
            break;

        //N3D Output
        case E_MV_Y:
        case E_MASK_M_Y:
        case E_SV_Y:
        case E_MASK_S_Y:

        //DPE Output
        case E_DMP_H:
        case E_CFM_H:
        case E_RESPO:
        case E_LDC:
            return StereoArea(272, 144, 32, 8, 16, 4).rotatedByModule();
            break;

        //OCC Output
        case E_MY_S:
        case E_DMH:

        //GF Output
        case E_DMG:
        case E_DMBG:

        //WMF Output
        case E_DMW:
            return StereoArea(240, 136);
            break;
        case E_DEPTH_MAP:
            return StereoArea(480, 270);
            break;
        //Bokeh Output
        case E_BOKEH_WROT: //VSDOF image
            switch(eScenario) {
                case eSTEREO_SCENARIO_PREVIEW:
                    return STEREO_AREA_ZERO;
                    break;
                case eSTEREO_SCENARIO_RECORD:
                    return StereoArea(1920, 1080);
                    break;
                case eSTEREO_SCENARIO_CAPTURE:
                    return StereoArea(3072, 1728);
                    break;
                default:
                    break;
            }
            break;
        case E_BOKEH_WDMA: //Clean image
            switch(eScenario) {
                case eSTEREO_SCENARIO_PREVIEW:
                case eSTEREO_SCENARIO_RECORD:
                    {
                        MSize previewSize(1920, 1080);
                        //TODO: get from metadata
                        return StereoArea(previewSize);
                    }
                    break;
                case eSTEREO_SCENARIO_CAPTURE:
                    return StereoArea(3072, 1728);
                    break;
                default:
                    break;
            }
            break;
        case E_BOKEH_3DNR:
            switch(eScenario) {
                case eSTEREO_SCENARIO_PREVIEW:
                case eSTEREO_SCENARIO_RECORD:
                    {
                        MSize previewSize(1920, 1080);
                        return StereoArea(previewSize);
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return STEREO_AREA_ZERO;
}

MSize
StereoSizeProvider::getSBSImageSize()
{
    Pass2SizeInfo pass2SizeInfo;
    getPass2SizeInfo(PASS2A_2, eSTEREO_SCENARIO_CAPTURE, pass2SizeInfo);
    MSize result = pass2SizeInfo.areaWDMA.size;
    result.w *= 2;

    return result;
}
