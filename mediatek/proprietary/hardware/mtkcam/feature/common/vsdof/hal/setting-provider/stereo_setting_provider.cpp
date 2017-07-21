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
#include <vsdof/hal/stereo_setting_provider.h>
#include <vsdof/hal/stereo_common.h>

#include <IHalSensor.h>
#include <ImageFormat.h>
#include <camera_custom_stereo.h>       // For CUST_STEREO_* definitions.
#include <Log.h>

#include <cutils/properties.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "StereoSettingProvider"

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

#define MY_LOGV_IF(cond, arg...)    if (cond) { MY_LOGV(arg); }
#define MY_LOGD_IF(cond, arg...)    if (cond) { MY_LOGD(arg); }
#define MY_LOGI_IF(cond, arg...)    if (cond) { MY_LOGI(arg); }
#define MY_LOGW_IF(cond, arg...)    if (cond) { MY_LOGW(arg); }
#define MY_LOGE_IF(cond, arg...)    if (cond) { MY_LOGE(arg); }

#define FUNC_START          MY_LOGD_IF(LOG_ENABLED, "+")
#define FUNC_END            MY_LOGD_IF(LOG_ENABLED, "-")

using namespace StereoHAL;

bool
StereoSettingProvider::getStereoSensorIndex(int32_t &main1Idx, int32_t &main2Idx)
{
    int32_t sensorCount = 3;
    IHalSensorList *sensorList = IHalSensorList::get();
    if (NULL == sensorList) {
        return false;
    }
    //
    sensorCount = sensorList->queryNumberOfSensors();
    //
    if(sensorCount < 2) {
        MY_LOGE("Invalid sensor count: %d", sensorCount);
        return false;
    } else if(sensorCount < 3) {
        main1Idx = 0;
        main2Idx = 1;
    } else {
        #if 2 == STEREO_CAM_FACING  //Stereo module at front
            main1Idx = 1;   //sub
            main2Idx = 2;   //main2
        #else
            main1Idx = 0;   //main1
            main2Idx = 2;   //main2
        #endif
    }
    //
//    MY_LOGD_IF(isLogEnabled(), "Main sensor idx %d, Main2 sensor idx %d", main1Idx, main2Idx);
    //
    return true;
}

bool
StereoSettingProvider::getStereoSensorDevIndex(int32_t &main1DevIdx, int32_t &main2DevIdx)
{
    int32_t main1Idx = 0;
    int32_t main2Idx = 0;
    if (!getStereoSensorIndex(main1Idx, main2Idx)) {
        return false;
    }
    IHalSensorList *sensorList = IHalSensorList::get();
    if (NULL == sensorList) {
        return false;
    }
    IHalSensor *pIHalSensor = sensorList->createSensor(LOG_TAG, main1Idx);
    if (NULL == pIHalSensor) {
        return false;
    }
    main1DevIdx = sensorList->querySensorDevIdx(main1Idx);
    main2DevIdx = sensorList->querySensorDevIdx(main2Idx);
    pIHalSensor->destroyInstance(LOG_TAG);
//    MY_LOGD_IF(isLogEnabled(), "Main sensor DEV idx %d, Main2 sensor DEV idx %d", main1DevIdx, main2DevIdx);
    //
    return true;
}

bool
StereoSettingProvider::setImageRatio(STEREO_RATIO_E eRatio)
{
    bool bSuccess = true;
    //
    imageRatio() = eRatio;
//    refocusSize().w = 256;
//    switch (eRatio) {
//    case eRatio_4_3:
//        imageResolution() = IMAGE_RESOLUTION_INFO_STRUCT(MSize(1920, 1440), MSize(1920, 1440), 4, 3);
//        refocusSize().h = 64; //16 align
//        break;
//    case eRatio_16_9:
//        imageResolution() = IMAGE_RESOLUTION_INFO_STRUCT(MSize(1920, 1080), MSize(1920, 1080), 16, 9);
//        refocusSize().h = 72; //For 16-align
//        break;
//    default:
//        bSuccess = false;
//    }
//    //
//    rectifySize().w = imageResolution().szMainCam.w + refocusSize().w;
//    rectifySize().h = imageResolution().szMainCam.h + refocusSize().h;
//    //
    return bSuccess;
}

bool
StereoSettingProvider::hasHWFE()
{
    static bool _hasHWFE = true;
    return _hasHWFE;
}

MUINT32
StereoSettingProvider::fefmBlockSize(const int FE_MODE)
{
    switch(FE_MODE)
    {
        case 0:
           return 32;
            break;
        case 1:
           return 16;
            break;
        case 2:
           return 8;
            break;
        default:
            break;
    }

    return 0;
}

bool
StereoSettingProvider::getStereoCameraFOV(float &mainFOV, float &main2FOV)
{
    getStereoFOV(STEREO_SENSOR_PROFILE_REAR_REAR, mainFOV, main2FOV);
    MY_LOGD_IF(isLogEnabled(), "FOV: %.1f / %.1f", mainFOV, main2FOV);
    return true;
}

float
StereoSettingProvider::getStereoCameraFOVRatio()
{
    static float fovRatio = 0.0f;
    if(0.0f == fovRatio) {
        float mainFOV, main2FOV;
        getStereoCameraFOV(mainFOV, main2FOV);
        const int FOV_DIFF = (int)(main2FOV - mainFOV);
        if(FOV_DIFF >= 20) {
            fovRatio = 1.4f;
        } else if(FOV_DIFF >= 15) {
            fovRatio = 1.3f;
        } else if(FOV_DIFF >= 10) {
            fovRatio = 1.2f;
        } else if(FOV_DIFF >= 5) {
            fovRatio = 1.1f;
        } else {
            fovRatio = 1.0f;
        }
    }

    return fovRatio;
}

unsigned int
StereoSettingProvider::getSensorRelativePosition()
{
    customSensorPos_STEREO_t SensorPos;
    SensorPos=getSensorPosSTEREO();

    return SensorPos.uSensorPos;
}

unsigned int
StereoSettingProvider::getJPSTransformFlag()
{
    return 0;//eTransform_None;
}

bool
StereoSettingProvider::enableLog()
{
    return setProperty(PROPERTY_ENABLE_LOG, 1);
}

bool
StereoSettingProvider::enableLog(const char *LOG_PROPERTY_NAME)
{
    return setProperty(LOG_PROPERTY_NAME, 1);
}

bool
StereoSettingProvider::disableLog()
{
    return setProperty(PROPERTY_ENABLE_LOG, 0);
}

bool
StereoSettingProvider::isLogEnabled()
{
    return (checkStereoProperty(PROPERTY_ENABLE_LOG, true, 0) == 1);
}

bool
StereoSettingProvider::isLogEnabled(const char *LOG_PROPERTY_NAME)
{
    return isLogEnabled() && (checkStereoProperty(LOG_PROPERTY_NAME, true, 0) == 1);
}

bool
StereoSettingProvider::isProfileLogEnabled()
{
    return isLogEnabled() || (checkStereoProperty(PROPERTY_ENABLE_PROFILE_LOG, true, 0) == 1);
}
