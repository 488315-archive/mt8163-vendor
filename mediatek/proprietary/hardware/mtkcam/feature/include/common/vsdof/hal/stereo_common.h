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
#ifndef _STEREO_COMMON_H_
#define _STEREO_COMMON_H_

#include <stdint.h>
#include <math.h>
#include <common.h>         //  mtkcam/common/include/common.h
#include <cutils/properties.h>
#include <Log.h>
#include <utils/include/Format.h>
#include <IHalSensor.h>
#include "stereo_setting_provider.h"

using namespace NSCam;

namespace StereoHAL {

#define WITH16ALIGN true
#define STEREO_PROPERTY_PREFIX  "debug.STEREO."

const MSize  MSIZE_ZERO(0, 0);
const MPoint MPOINT_ZERO(0, 0);

enum ENUM_STEREO_SCENARIO
{
    eSTEREO_SCENARIO_UNKNOWN,   //sensor scenario: SENSOR_SCENARIO_ID_UNNAMED_START
    eSTEREO_SCENARIO_PREVIEW,   //sensor scenario: SENSOR_SCENARIO_ID_NORMAL_PREVIEW
    eSTEREO_SCENARIO_RECORD,    //sensor scenario: SENSOR_SCENARIO_ID_NORMAL_VIDEO
    eSTEREO_SCENARIO_CAPTURE    //sensor scenario: SENSOR_SCENARIO_ID_NORMAL_CAPTURE
};

inline int getSensorSenario(ENUM_STEREO_SCENARIO eScenario)
{
    switch(eScenario) {
        case eSTEREO_SCENARIO_PREVIEW:
        case eSTEREO_SCENARIO_RECORD:
            return SENSOR_SCENARIO_ID_NORMAL_PREVIEW;
            break;
        // case eSTEREO_SCENARIO_RECORD:
        //     return SENSOR_SCENARIO_ID_NORMAL_VIDEO;
        //     break;
        case eSTEREO_SCENARIO_CAPTURE:
            return SENSOR_SCENARIO_ID_NORMAL_CAPTURE;
            break;
        default:
            break;
    }

    return SENSOR_SCENARIO_ID_UNNAMED_START;
}

enum ENUM_STEREO_SENSOR
{
    eSTEREO_SENSOR_UNKNOWN,
    eSTEREO_SENSOR_MAIN1,
    eSTEREO_SENSOR_MAIN2
};

enum ENUM_STEREO_IMG_RATIO
{
    eSTEREO_RATIO_16_9,
    eSTEREO_RATIO_4_3,
};

struct StereoArea {
    MSize size;
    MSize padding;
    MPoint startPt;

    StereoArea() {
        ::memset(this, 0, sizeof(StereoArea));
    }

    ~StereoArea() {}

    StereoArea(MUINT32 w, MUINT32 h, MUINT32 paddingX=0, MUINT32 paddingY=0, MUINT32 startX = 0, MUINT32 startY = 0) {
        size.w = w, size.h = h;
        padding.w = paddingX, padding.h = paddingY;
        startPt.x = startX, startPt.y = startY;
    }

    StereoArea(MSize sz, MSize p=MSIZE_ZERO, MPoint pt = MPOINT_ZERO) {
        size = sz;
        padding = p;
        startPt = pt;
    }

    StereoArea &operator=(const StereoArea &rhs) {
        size = rhs.size;
        padding = rhs.padding;
        startPt = rhs.startPt;

        return *this;
    }

    bool operator==(const StereoArea &rhs) {
        if(size != rhs.size) return false;
        if(padding != rhs.padding) return false;
        if(startPt != rhs.startPt) return false;

        return true;
    }

    StereoArea(const StereoArea &rhs) {
        size = rhs.size;
        padding = rhs.padding;
        startPt = rhs.startPt;
    }

    operator MSize() const {
        return size;
    }

    MSize contentSize() const {
        return (size - padding);
    }

    StereoArea rotatedByModule() const {
        if( StereoSettingProvider::getModuleRotation() == eRotate_0 ||
            StereoSettingProvider::getModuleRotation() == eRotate_180 )
        {
            return *this;
        }

        StereoArea rotatedArea(size.h, size.w, padding.w, padding.h);
        rotatedArea.startPt.x = padding.w/2;
        rotatedArea.startPt.y = padding.h/2;
        return rotatedArea;
    }

    StereoArea &apply16Align() {
        MSize contentSize = size - padding;

        MSize szPadding = MSize(size.w % 16, size.h % 16);
        if(szPadding.w != 0) {
            size.w -= szPadding.w;
            padding.w = size.w - contentSize.w;
            startPt.x = padding.w>>1;
        }

        if(szPadding.h != 0) {
            size.h -= szPadding.h;
            padding.h = size.h - contentSize.h;
            startPt.y = padding.h>>1;
        }

        return *this;
    }

    void print() {
#ifdef GTEST
        printf("[StereoArea]Size(%dx%d), Padding(%dx%d), StartPt(%d, %d)\n",
                size.w, size.h, padding.w, padding.h, startPt.x, startPt.y);
#else
        ALOGD("[StereoArea]Size(%dx%d), Padding(%dx%d), StartPt(%d, %d)",
               size.w, size.h, padding.w, padding.h, startPt.x, startPt.y);
#endif
    }
};

const StereoArea STEREO_AREA_ZERO;

/* Sample code:
 * struct timespec t_start, t_end, t_result;
 * clock_gettime(CLOCK_MONOTONIC, &t_start);
 * ...
 * clock_gettime(CLOCK_MONOTONIC, &t_end);
 * t_result = timeDiff(t_start, t_end);
 * MY_LOGD("Runnning Time: %lu.%.9lu", t_result.tv_sec, t_result.tv_nsec);
 */
inline struct timespec timeDiff( struct timespec start, struct timespec end)
{
    struct timespec t_result;

    if( ( end.tv_nsec - start.tv_nsec ) < 0) {
        t_result.tv_sec = end.tv_sec - start.tv_sec - 1;
        t_result.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        t_result.tv_sec = end.tv_sec - start.tv_sec;
        t_result.tv_nsec = end.tv_nsec - start.tv_nsec;
    }

    return t_result;
}

/**
 * @param PROPERTY_NAME The property to query, e.g. "debug.STEREO.enable_verify"
 * @param HAS_DEFAULT If ture, refer to DEFAULT value
 * @param DEFAULT Default value of the property. If not set, it will be 0.
 * @return: -1: property not been set; otherwise the property value; if HAS_DEFAULT return >= 0
 */
inline int checkStereoProperty(const char *PROPERTY_NAME, const bool HAS_DEFAULT=true, const int DEFAULT=0)
{
    char val[PROPERTY_VALUE_MAX];
    ::memset(val, sizeof(char)*PROPERTY_VALUE_MAX, 0);

    int len = 0;
    if(HAS_DEFAULT) {
        char strDefault[PROPERTY_VALUE_MAX];
        sprintf(strDefault, "%d", DEFAULT);
        len = property_get(PROPERTY_NAME, val, strDefault);
    } else {
        len = property_get(PROPERTY_NAME, val, NULL);
    }

    if(len <= 0) {
        return -1; //property not exist
    }

    //return (!strcmp(val, "1"));
    return atoi(val);
}

inline bool setProperty(const char *PROPERTY_NAME, int val)
{
    if(NULL == PROPERTY_NAME) {
        return false;
    }

    char value[PROPERTY_VALUE_MAX];
    sprintf(value, "%d", val);
    int ret = property_set(PROPERTY_NAME, value);

    return (0 == ret);
}

};
#endif