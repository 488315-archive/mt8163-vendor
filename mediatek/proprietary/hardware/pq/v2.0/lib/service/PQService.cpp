#define LOG_TAG "PQ"
#define MTK_LOG_ENABLE 1
#include <cutils/log.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <cutils/properties.h>

/*PQService */
#include <dlfcn.h>
#include <math.h>
#include <utils/SortedVector.h>
#include <binder/PermissionCache.h>
#include <android/native_window.h>
#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>
#include <ui/DisplayInfo.h>
#include <cutils/memory.h>

 /*PQService */
#include <linux/disp_session.h>

#include "ddp_drv.h"
#include "cust_gamma.h"
#include "cust_color.h"
#include "cust_tdshp.h"
#include <dlfcn.h>
/*PQService */
#include <binder/BinderService.h>
#include <PQService.h>
#include <BluLight_Defender.h>


/*PQService */
namespace android {

#define DISP_DEV_NODE_PATH "/dev/mtk_disp_mgr"

// Property of blue light filter strength
#define MTK_BLUELIGHT_STRENGTH_PROPERTY_NAME "persist.sys.bluelight.strength"

#define PQ_LOGD(fmt, arg...) ALOGD(fmt, ##arg)
#define PQ_LOGE(fmt, arg...) ALOGE(fmt, ##arg)

#define max( a, b )            (((a) > (b)) ? (a) : (b))

/*cust PQ default setting*/

#define MEMORY_SIZE 1*1024

static DISP_PQ_MAPPING_PARAM pqparam_mapping =
{
    image:80,
    video:100,
    camera:20,
} ;

static DISP_PQ_PARAM pqparam_table[PQ_PARAM_TABLE_SIZE]=
{
    //std_image
    {
        u4SHPGain:2,
        u4SatGain:4,
        u4PartialY:0,
        u4HueAdj:{9,9,12,12},
        u4SatAdj:{0,6,10,10},
        u4Contrast:4,
        u4Brightness:4,
        u4Ccorr:0
    },

    //std_video
    {
        u4SHPGain:3,
        u4SatGain:4,
        u4PartialY:0,
        u4HueAdj:{9,9,12,12},
        u4SatAdj:{0,6,12,12},
        u4Contrast:4,
        u4Brightness:4,
        u4Ccorr:0
    },

    //std_camera
    {
        u4SHPGain:2,
        u4SatGain:4,
        u4PartialY:0,
        u4HueAdj:{9,9,12,12},
        u4SatAdj:{0,6,10,10},
        u4Contrast:4,
        u4Brightness:4,
        u4Ccorr:0
    },

    //viv_image
    {
        u4SHPGain:2,
        u4SatGain:9,
        u4PartialY:0,
        u4HueAdj:{9,9,12,12},
        u4SatAdj:{16,16,16,16},
        u4Contrast:4,
        u4Brightness:4,
        u4Ccorr:0
    },

    //viv_video
    {
        u4SHPGain:3,
        u4SatGain:9,
        u4PartialY:0,
        u4HueAdj:{9,9,12,12},
        u4SatAdj:{16,16,18,18},
        u4Contrast:4,
        u4Brightness:4,
        u4Ccorr:0
    },

    //viv_camera
    {
        u4SHPGain:2,
        u4SatGain:4,
        u4PartialY:0,
        u4HueAdj:{9,9,12,12},
        u4SatAdj:{0,6,10,10},
        u4Contrast:4,
        u4Brightness:4,
        u4Ccorr:0
    },

    //pqparam_usr
    {
        u4SHPGain:2,
        u4SatGain:9,
        u4PartialY:0,
        u4HueAdj:{9,9,12,12},
        u4SatAdj:{16,16,16,16},
        u4Contrast:4,
        u4Brightness:4,
        u4Ccorr:0
    }
};

#ifdef TDSHP_2_0
static DISP_PQ_DS_PARAM_EX g_PQ_DS_Param = {
param:
    {1, -4, 1024, -4, 1024,
     1, 400, 200, 1600, 800,
     128, 8, 4, 12, 16,
     8, 24, -8, -4, -12,
     0, 0, 0,
     8, 4, 12, 16, 8, 24, -8, -4, -12,
     8, 4, 12, 16, 8, 24, -8, -4, -12,
     8, 4, 12, 8, 4, 12,
     1,
     4096, 2048, 1024, 34, 35, 51, 50, -1, -2, -4, 0, -4, 0}
};
static MDP_TDSHP_REG_EX g_tdshp_reg = {
param:
    {0x10, 0x20, 0x10, 0x4, 0x2, 0x20, 0x3,
     0x2, 0x4, 0x10, 0x3, 0x2, 0x4, 0x10, 0x3,
     0x10, 0x10, 0x10, 0x8, 0x2}
};
#else
static DISP_PQ_DS_PARAM_EX g_PQ_DS_Param = {
param:
    {1, -4, 1024, -4, 1024,
     1, 400, 200, 1600, 800,
     128, 8, 4, 12, 16,
     8, 24, -8, -4, -12,
     0, 0, 0}
};

static MDP_TDSHP_REG_EX g_tdshp_reg = {
param:
    {0x10, 0x20, 0x10, 0x4, 0x2, 0x20, 0x3}
};
#endif

static PQ_RSZ_PARAM rszparam = {
param:
    {0, 10, 10, 1, 1, 1, 1, 1, 1,
     1, 24,
     1024, 2048, 4096, 3, 8, 0, 31,
     1024, 1536, 2048, 0, -7, 0, 15,
     4, 1024, 2048, 4096, 0, -2, 0, 255,
     8, 1024, 2048, 4096, 0, -7, 0, 31}

};

template <typename _T>
static char *printIntArray(char *ori_buffer, int bsize, const _T *arr, int length)
{
    char *buffer = ori_buffer;
    int n;

    n = 4;
    while (length > 0 && bsize > 1) {
        while (length < n)
            n >>= 1;
        if (n == 0) break; // length == 0

        // We print n items at one time
        int pr_len;
        if (n == 4) {
            pr_len = snprintf(buffer, bsize, " %4d %4d %4d %4d",
                (int)(arr[0]), (int)(arr[1]), (int)(arr[2]), (int)(arr[3]));
        } else if (n == 2) {
            pr_len = snprintf(buffer, bsize, " %4d %4d", (int)(arr[0]), (int)(arr[1]));
        } else {
            pr_len = snprintf(buffer, bsize, " %4d", (int)(arr[0]));
        }

        buffer += pr_len;
        bsize -= pr_len;
        arr += n;
        length -= n;
    }

    ori_buffer[bsize - 1] = '\0';

    return ori_buffer;
}


static inline int valueOf(const char16_t ch)
{
    if (L'0' <= ch && ch <= L'9')
        return (ch - L'0');
    else if (L'a' <= ch && ch <= L'z')
        return (ch - L'a') + 10;
    else if (L'A' <= ch && ch <= L'Z')
        return (ch - L'A') + 10;

    return 0;
}


static int asInt(const String16& str)
{
    int val = 0;
    bool negative = false;
    int base = 10;

    const char16_t *char_p = str.string();

    if (*char_p == L'-') {
        negative = true;
        char_p++;
    } else if (*char_p == '+') {
        negative = false;
        char_p++;
    } else
        negative = false;

    if (*char_p == L'0' && *(char_p + 1) == L'x') {
        base = 16;
        char_p += 2;
    }

    for ( ; *char_p != L'\0'; char_p++) {
        val = val * base + valueOf(*char_p);
    }

    return (negative ? -val : val);
}


PQService::PQService()
{
    PQ_LOGD("[PQ_SERVICE] PQService constructor");

    m_drvID = open(PQ_DEVICE_NODE, O_RDONLY, 0);

    if (m_drvID < 0)
    {
        PQ_LOGE("[PQ_SERVICE] open device fail!!");
    }

    // set all feature default off
    memset(m_bFeatureSwitch, 0, sizeof(uint32_t) * PQ_FEATURE_MAX);

    // update it by each feature
#ifndef DISP_COLOR_OFF
    m_bFeatureSwitch[DISPLAY_COLOR] = 1;
#endif
#ifdef MDP_COLOR_ENABLE
    m_bFeatureSwitch[CONTENT_COLOR] = 1;
#endif
    m_bFeatureSwitch[DYNAMIC_SHARPNESS] = 1;

    PQ_LOGD("[PQ_SERVICE] PQService constructor");

    blueLight = new BluLightDefender;
}

PQService::~PQService()
{
    close(m_drvID);
    delete blueLight;
}

void PQService::calcPQStrength(DISP_PQ_PARAM *pqparam_dst, DISP_PQ_PARAM *pqparam_src, int percentage)
{
    pqparam_dst->u4SatGain = pqparam_src->u4SatGain * percentage / 100;
    pqparam_dst->u4PartialY  = pqparam_src->u4PartialY;
    pqparam_dst->u4HueAdj[0] = pqparam_src->u4HueAdj[0];
    pqparam_dst->u4HueAdj[1] = pqparam_src->u4HueAdj[1];
    pqparam_dst->u4HueAdj[2] = pqparam_src->u4HueAdj[2];
    pqparam_dst->u4HueAdj[3] = pqparam_src->u4HueAdj[3];
    pqparam_dst->u4SatAdj[0] = pqparam_src->u4SatAdj[0];
    pqparam_dst->u4SatAdj[1] = pqparam_src->u4SatAdj[1] * percentage / 100;
    pqparam_dst->u4SatAdj[2] = pqparam_src->u4SatAdj[2] * percentage / 100;
    pqparam_dst->u4SatAdj[3] = pqparam_src->u4SatAdj[3] * percentage / 100;
    pqparam_dst->u4Contrast = pqparam_src->u4Contrast * percentage / 100;
    pqparam_dst->u4Brightness = pqparam_src->u4Brightness * percentage / 100;
    pqparam_dst->u4SHPGain = pqparam_src->u4SHPGain * percentage / 100;
    pqparam_dst->u4Ccorr = pqparam_src->u4Ccorr;
}

status_t PQService::getColorRegion(DISP_PQ_WIN_PARAM *win_param)
{
    Mutex::Autolock _l(mLock);
    //for MDP split demo window, MDP should update this per frame???
    win_param->split_en = mdp_win_param.split_en;
    win_param->start_x = mdp_win_param.start_x;
    win_param->start_y = mdp_win_param.start_y;
    win_param->end_x = mdp_win_param.end_x;
    win_param->end_y = mdp_win_param.end_y;
    return NO_ERROR;

}

status_t PQService::setColorRegion(int32_t split_en,int32_t start_x,int32_t start_y,int32_t end_x,int32_t end_y)
{
    Mutex::Autolock _l(mLock);
#ifndef DISP_COLOR_OFF

    if (m_drvID < 0)
    {
        PQ_LOGE("[PQ_SERVICE] open device fail!!");
        return UNKNOWN_ERROR ;
    }
    mdp_win_param.split_en = split_en;
    mdp_win_param.start_x = start_x;
    mdp_win_param.start_y = start_y;
    mdp_win_param.end_x = end_x;
    mdp_win_param.end_y = end_y;
    ioctl(m_drvID, DISP_IOCTL_PQ_SET_WINDOW, &mdp_win_param);

    //for MDP split demo window, MDP should update this per frame???
#endif
    return NO_ERROR;

}

status_t PQService::setPQMode(int32_t mode)
{
    Mutex::Autolock _l(mLock);
    PQMode = mode;
    char value[PROPERTY_VALUE_MAX];
    snprintf(value, PROPERTY_VALUE_MAX, "%d\n", mode);
    property_set(PQ_PIC_MODE_PROPERTY_STR, value);
    PQ_LOGD("[PQ_SERVICE] property set... picture mode[%d]", mode);
#ifndef DISP_COLOR_OFF
    int i ;
    int percentage = pqparam_mapping.image;  //default scenario = image

    if (m_drvID < 0)
    {
        PQ_LOGE("[PQ_SERVICE] open device fail!!");
        return UNKNOWN_ERROR ;
    }


    if (mode == PQ_PIC_MODE_STANDARD || mode == PQ_PIC_MODE_VIVID)
    {
        PQ_LOGD("[PQ_SERVICE] --DISP_IOCTL_SET_PQPARAM, gsat[%d], cont[%d], bri[%d] ", pqparam_table[(mode) * PQ_SCENARIO_COUNT + 1].u4SatGain, pqparam_table[(mode) * PQ_SCENARIO_COUNT + 1].u4Contrast, pqparam_table[(mode) * PQ_SCENARIO_COUNT + 1].u4Brightness);
        PQ_LOGD("[PQ_SERVICE] --DISP_IOCTL_SET_PQPARAM, hue0[%d], hue1[%d], hue2[%d], hue3[%d] ", pqparam_table[(mode) * PQ_SCENARIO_COUNT + 1].u4HueAdj[0], pqparam_table[(mode) * PQ_SCENARIO_COUNT + 1].u4HueAdj[1], pqparam_table[(mode) * PQ_SCENARIO_COUNT + 1].u4HueAdj[2], pqparam_table[(mode) * PQ_SCENARIO_COUNT + 1].u4HueAdj[3]);
        PQ_LOGD("[PQ_SERVICE] --DISP_IOCTL_SET_PQPARAM, sat0[%d], sat1[%d], sat2[%d], sat3[%d] ", pqparam_table[(mode) * PQ_SCENARIO_COUNT + 1].u4SatAdj[0], pqparam_table[(mode) * PQ_SCENARIO_COUNT + 1].u4SatAdj[1], pqparam_table[(mode) * PQ_SCENARIO_COUNT + 1].u4SatAdj[2], pqparam_table[(mode) * PQ_SCENARIO_COUNT + 1].u4SatAdj[3]);

        memcpy(&pqparam, &pqparam_table[(mode) * PQ_SCENARIO_COUNT + 0], sizeof(DISP_PQ_PARAM));

        setPQParamWithFilter(m_drvID, &pqparam);
    }
    else if (mode == PQ_PIC_MODE_USER_DEF)
    {
        // USER MODE
        //memcpy(&pqparam_user_def, &pqparam_usr, sizeof(pqparam_usr));   // default value from standard setting.

        property_get(PQ_TDSHP_PROPERTY_STR, value, PQ_TDSHP_INDEX_DEFAULT);
        i = atoi(value);
        PQ_LOGD("[PQ_SERVICE] property get... tdshp[%d]", i);
        pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT].u4SHPGain = i;

        property_get(PQ_GSAT_PROPERTY_STR, value, PQ_GSAT_INDEX_DEFAULT);
        i = atoi(value);
        PQ_LOGD("[PQ_SERVICE] property get... gsat[%d]", i);
        pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT].u4SatGain = i;

        property_get(PQ_CONTRAST_PROPERTY_STR, value, PQ_CONTRAST_INDEX_DEFAULT);
        i = atoi(value);
        PQ_LOGD("[PQ_SERVICE] property get... contrast[%d]", i);
        pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT].u4Contrast = i;

        property_get(PQ_PIC_BRIGHT_PROPERTY_STR, value, PQ_PIC_BRIGHT_INDEX_DEFAULT);
        i = atoi(value);
        PQ_LOGD("[PQ_SERVICE] property get... pic bright[%d]", i);
        pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT].u4Brightness = i;

        calcPQStrength(&pqparam, &pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT], percentage);

        setPQParamWithFilter(m_drvID, &pqparam);

        PQ_LOGD("[PQ_SERVICE] --DISP_IOCTL_SET_PQPARAM, shp[%d], gsat[%d], cont[%d], bri[%d] ", pqparam.u4SHPGain, pqparam.u4SatGain, pqparam.u4Contrast, pqparam.u4Brightness);
        PQ_LOGD("[PQ_SERVICE] --DISP_IOCTL_SET_PQPARAM, hue0[%d], hue1[%d], hue2[%d], hue3[%d] ", pqparam.u4HueAdj[0], pqparam.u4HueAdj[1], pqparam.u4HueAdj[2], pqparam.u4HueAdj[3]);
        PQ_LOGD("[PQ_SERVICE] --DISP_IOCTL_SET_PQPARAM, sat0[%d], sat1[%d], sat2[%d], sat3[%d] ", pqparam.u4SatAdj[0], pqparam.u4SatAdj[1], pqparam.u4SatAdj[2], pqparam.u4SatAdj[3]);

    }
    else
    {
        PQ_LOGE("[PQ_SERVICE] unknown picture mode!!");
        PQ_LOGD("[PQ_SERVICE] --DISP_IOCTL_SET_PQPARAM, gsat[%d], cont[%d], bri[%d] ", pqparam_table[0].u4SatGain, pqparam_table[0].u4Contrast, pqparam_table[0].u4Brightness);
        PQ_LOGD("[PQ_SERVICE] --DISP_IOCTL_SET_PQPARAM, hue0[%d], hue1[%d], hue2[%d], hue3[%d] ", pqparam_table[0].u4HueAdj[0], pqparam_table[0].u4HueAdj[1], pqparam_table[0].u4HueAdj[2], pqparam_table[0].u4HueAdj[3]);
        PQ_LOGD("[PQ_SERVICE] --DISP_IOCTL_SET_PQPARAM, sat0[%d], sat1[%d], sat2[%d], sat3[%d] ", pqparam_table[0].u4SatAdj[0], pqparam_table[0].u4SatAdj[1], pqparam_table[0].u4SatAdj[2], pqparam_table[0].u4SatAdj[3]);

        memcpy(&pqparam, &pqparam_table[0], sizeof(DISP_PQ_PARAM));

        setPQParamWithFilter(m_drvID, &pqparam);
    }
#endif



    return NO_ERROR;
}

status_t PQService::setPQIndex(int32_t level, int32_t  scenario, int32_t  tuning_mode, int32_t index)
{
    Mutex::Autolock _l(mLock);
    char value[PROPERTY_VALUE_MAX];
    int percentage;
    DISP_PQ_PARAM *pqparam_image_ptr;
    DISP_PQ_PARAM *pqparam_video_ptr;

    if (m_drvID < 0)
    {
        PQ_LOGE("[PQ_SERVICE] open device fail!!");
        return UNKNOWN_ERROR ;
    }

    PQScenario = scenario;
    // assume scenario is image mode, DISP is always on
    if (PQScenario == SCENARIO_PICTURE) {
        percentage = pqparam_mapping.image;
    }
    else  if(PQScenario == SCENARIO_VIDEO) {
        percentage = pqparam_mapping.video;
    }
    else  if(PQScenario == SCENARIO_ISP_PREVIEW){
        percentage = pqparam_mapping.camera;
    }
    else {
        percentage = pqparam_mapping.image;
        PQ_LOGE("[PQ_SERVICE] PQService : getColorIndex, invalid scenario\n");
    }

    if (PQMode == PQ_PIC_MODE_STANDARD || PQMode == PQ_PIC_MODE_VIVID) {
        pqparam_image_ptr=&pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 0];
        pqparam_video_ptr=&pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 1];
        memcpy(&pqparam, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 0], sizeof(DISP_PQ_PARAM));
    }
    else if (PQMode == PQ_PIC_MODE_USER_DEF) {
        pqparam_image_ptr=&pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT];
        pqparam_video_ptr=&pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT];
        calcPQStrength(&pqparam, &pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT], percentage);
    }
    else {
        PQ_LOGE("[PQ_SERVICE] PQService : Unknown PQMode\n");
        pqparam_image_ptr=&pqparam_table[0];
        pqparam_video_ptr=&pqparam_table[1];
    }

    PQ_LOGD("[PQ_SERVICE] setPQIndex : PQMode = %d, PQScenario = %d, level = %d, percentage = %d\n",PQMode,PQScenario,level,percentage);
    PQ_LOGD("[PQ_SERVICE] setPQIndex--Get_PQ, gsat[%d], cont[%d], bri[%d] ", pqparam.u4SatGain, pqparam.u4Contrast, pqparam.u4Brightness);
    PQ_LOGD("[PQ_SERVICE] setPQIndex--Get_PQ, hue0[%d], hue1[%d], hue2[%d], hue3[%d] ", pqparam.u4HueAdj[0], pqparam.u4HueAdj[1], pqparam.u4HueAdj[2], pqparam.u4HueAdj[3]);
    PQ_LOGD("[PQ_SERVICE] setPQIndex--Get_PQ, sat0[%d], sat1[%d], sat2[%d], sat3[%d] ", pqparam.u4SatAdj[0], pqparam.u4SatAdj[1], pqparam.u4SatAdj[2], pqparam.u4SatAdj[3]);



    switch (index) {
        case SET_PQ_SHP_GAIN:
            {
                if (PQMode == PQ_PIC_MODE_USER_DEF){
                    pqparam.u4SHPGain = level * percentage / 100;
                }
                else{
                    pqparam.u4SHPGain = level;
                }
                pqparam_image_ptr->u4SHPGain = level;
                PQ_LOGD("[PQ_SERVICE] setPQIndex SET_PQ_SHP_GAIN...[%d]\n", pqparam.u4SHPGain);
            }
            break;
        case SET_PQ_SAT_GAIN:
            {
                if (PQMode == PQ_PIC_MODE_USER_DEF){
                    pqparam.u4SatGain = level * percentage / 100;
                }
                else{
                    pqparam.u4SatGain = level;
                }
                pqparam_image_ptr->u4SatGain = level;
                PQ_LOGD("[PQ_SERVICE] setPQIndex SET_PQ_SAT_GAIN...[%d]\n", pqparam.u4SatGain);
            }
            break;
        case SET_PQ_LUMA_ADJ:
            {
                pqparam.u4PartialY= (level);
                pqparam_image_ptr->u4PartialY= level;
                PQ_LOGD("[PQ_SERVICE] setPQIndex SET_PQ_LUMA_ADJ...[%d]\n",pqparam.u4PartialY);
            }
            break;
        case  SET_PQ_HUE_ADJ_SKIN:
            {
                pqparam.u4HueAdj[1]= (level) ;
                pqparam_image_ptr->u4HueAdj[1]= level;
                PQ_LOGD("[PQ_SERVICE] setPQIndex SET_PQ_HUE_ADJ_SKIN...[%d]\n",pqparam.u4HueAdj[1]);
            }
            break;
        case  SET_PQ_HUE_ADJ_GRASS:
            {
                pqparam.u4HueAdj[2] = level;
                pqparam_image_ptr->u4HueAdj[2]= level;
                PQ_LOGD("[PQ_SERVICE] setPQIndex SET_PQ_HUE_ADJ_GRASS...[%d]\n",pqparam.u4HueAdj[2]);
            }
            break;
        case  SET_PQ_HUE_ADJ_SKY:
            {
                pqparam.u4HueAdj[3] = level;
                pqparam_image_ptr->u4HueAdj[3]= level;
                PQ_LOGD("[PQ_SERVICE] setPQIndex SET_PQ_HUE_ADJ_SKY...[%d]\n",pqparam.u4HueAdj[3]);
            }
            break;
        case SET_PQ_SAT_ADJ_SKIN:
            {
                if (PQMode == PQ_PIC_MODE_USER_DEF){
                    pqparam.u4SatAdj[1]= (level) * percentage / 100;
                }
                else{
                    pqparam.u4SatAdj[1] = level;
                }
                pqparam_image_ptr->u4SatAdj[1]= level;
                PQ_LOGD("[PQ_SERVICE] setPQIndex SET_PQ_SAT_ADJ_SKIN...[%d]\n",pqparam.u4SatAdj[1]);
            }
            break;
        case SET_PQ_SAT_ADJ_GRASS:
            {
                if (PQMode == PQ_PIC_MODE_USER_DEF){
                    pqparam.u4SatAdj[2]= (level) * percentage / 100;
                }
                else{
                    pqparam.u4SatAdj[2] = level;
                }
                pqparam_image_ptr->u4SatAdj[2]= level;
                PQ_LOGD("[PQ_SERVICE] setPQIndex SET_PQ_SAT_ADJ_GRASS...[%d]\n",pqparam.u4SatAdj[2] );
            }
            break;
        case SET_PQ_SAT_ADJ_SKY:
            {
                if (PQMode == PQ_PIC_MODE_USER_DEF){
                    pqparam.u4SatAdj[3]= (level) * percentage / 100;
                }
                else{
                    pqparam.u4SatAdj[3] = level;
                }
                pqparam_image_ptr->u4SatAdj[3]= level;
                PQ_LOGD("[PQ_SERVICE] setPQIndex SET_PQ_SAT_ADJ_GRASS...[%d]\n",pqparam.u4SatAdj[3] );
            }
            break;
        case SET_PQ_CONTRAST:
            {
                if (PQMode == PQ_PIC_MODE_USER_DEF){
                    pqparam.u4Contrast= (level) * percentage / 100 ;
                }
                else{
                    pqparam.u4Contrast= level;
                }
                pqparam_image_ptr->u4Contrast= level;
                PQ_LOGD("[PQ_SERVICE] setPQIndex SET_PQ_CONTRAST...[%d]\n", pqparam.u4Contrast);
            }
            break;
        case SET_PQ_BRIGHTNESS:
            {
                if (PQMode == PQ_PIC_MODE_USER_DEF){
                    pqparam.u4Brightness= (level) * percentage / 100 ;
                }
                else{
                    pqparam.u4Brightness= level;
                }
                pqparam_image_ptr->u4Brightness= level;
                PQ_LOGD("[PQ_SERVICE] setPQIndex SET_PQ_BRIGHTNESS...[%d]\n", pqparam.u4Brightness);
            }
            break;
        default:
            PQ_LOGD("[PQ_SERVICE] setPQIndex default case...\n");
    }
    // if in Gallery PQ tuning mode, sync video param with image param
    if(tuning_mode == TDSHP_FLAG_TUNING)
    {
        *pqparam_video_ptr=*pqparam_image_ptr;
    }

    PQ_LOGD("[PQ_SERVICE] setPQIndex--Set_PQ_Userdef, gsat[%d], cont[%d], bri[%d] ", pqparam.u4SatGain, pqparam.u4Contrast, pqparam.u4Brightness);
    PQ_LOGD("[PQ_SERVICE] setPQIndex--Set_PQ_Userdef, hue0[%d], hue1[%d], hue2[%d], hue3[%d] ", pqparam.u4HueAdj[0], pqparam.u4HueAdj[1], pqparam.u4HueAdj[2], pqparam.u4HueAdj[3]);
    PQ_LOGD("[PQ_SERVICE] setPQIndex--Set_PQ_Userdef, sat0[%d], sat1[%d], sat2[%d], sat3[%d] ", pqparam.u4SatAdj[0], pqparam.u4SatAdj[1], pqparam.u4SatAdj[2], pqparam.u4SatAdj[3]);

    setPQParamWithFilter(m_drvID, &pqparam);

    return NO_ERROR;
}

status_t PQService::getMappedColorIndex(DISP_PQ_PARAM *index, int32_t scenario, int32_t mode)
{
    Mutex::Autolock _l(mLock);
    PQScenario = scenario;
    int percentage = 0;
    //scenario will be passed from MDP, return corresponding param of scenario automatically

    PQ_LOGD("[PQ_SERVICE] PQService : PQScenario = %d, PQMode = %d\n",PQScenario,PQMode);
    // mdp_pqparam_standard/vivid will be added

    if (PQMode == PQ_PIC_MODE_STANDARD || PQMode == PQ_PIC_MODE_VIVID) {

        if(PQScenario ==  SCENARIO_PICTURE) {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 0], sizeof(DISP_PQ_PARAM));
        }
        else  if (PQScenario == SCENARIO_VIDEO) {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 1], sizeof(DISP_PQ_PARAM));
        }
        else  if (PQScenario == SCENARIO_ISP_PREVIEW) {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 2], sizeof(DISP_PQ_PARAM));
        }
        else {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 0], sizeof(DISP_PQ_PARAM));
            PQ_LOGD("[PQ_SERVICE] PQService : getMappedColorIndex, invalid scenario\n");
        }
    }
    else if (PQMode == PQ_PIC_MODE_USER_DEF) {
        if (PQScenario ==  SCENARIO_PICTURE) {
            percentage = pqparam_mapping.image;
        }
        else  if (PQScenario == SCENARIO_VIDEO) {
            percentage = pqparam_mapping.video;
        }
        else  if (PQScenario == SCENARIO_ISP_PREVIEW) {
            percentage = pqparam_mapping.camera;
        }
        else {
            percentage = pqparam_mapping.image;
            PQ_LOGD("[PQ_SERVICE] PQService : getMappedColorIndex, invalid scenario\n");
        }

        calcPQStrength(index, &pqparam, percentage);
    }
    else {
        memcpy(index, &pqparam_table[0], sizeof(DISP_PQ_PARAM));
        PQ_LOGD("[PQ_SERVICE] PQService : getMappedColorIndex, invalid mode\n");
    }

    return NO_ERROR;
}

status_t PQService::getMappedTDSHPIndex(DISP_PQ_PARAM *index, int32_t scenario, int32_t mode)
{

    Mutex::Autolock _l(mLock);
    PQScenario = scenario;
    int percentage = 0;

    PQ_LOGD("[PQ_SERVICE] PQService : PQScenario = %d, PQMode = %d\n",PQScenario,PQMode);

    //scenario will be passed from MDP, return corresponding param of scenario automatically
    // mdp_pqparam_standard/vivid will be added
    if (PQMode == PQ_PIC_MODE_STANDARD || PQMode == PQ_PIC_MODE_VIVID) {

        if (PQScenario ==  SCENARIO_PICTURE) {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 0], sizeof(DISP_PQ_PARAM));
        }
        else  if (PQScenario == SCENARIO_VIDEO) {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 1], sizeof(DISP_PQ_PARAM));
        }
        else  if (PQScenario == SCENARIO_ISP_PREVIEW) {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 2], sizeof(DISP_PQ_PARAM));
        }
        else {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 0], sizeof(DISP_PQ_PARAM));
            PQ_LOGD("[PQ_SERVICE] PQService : getMappedTDSHPIndex, invalid scenario\n");
        }
    }
    else if (PQMode == PQ_PIC_MODE_USER_DEF) {
        if (PQScenario ==  SCENARIO_PICTURE) {
            percentage = pqparam_mapping.image;
        }
        else  if (PQScenario == SCENARIO_VIDEO) {
            percentage = pqparam_mapping.video;
        }
        else  if (PQScenario == SCENARIO_ISP_PREVIEW) {
            percentage = pqparam_mapping.camera;
        }
        else {
            percentage = pqparam_mapping.image;
            PQ_LOGD("[PQ_SERVICE] PQService : getMappedTDSHPIndex, invalid scenario\n");
        }

        calcPQStrength(index, &pqparam, percentage);
    }
    else {
        memcpy(index, &pqparam_table[0], sizeof(DISP_PQ_PARAM));
        PQ_LOGD("[PQ_SERVICE] PQService : getMappedTDSHPIndex, invalid mode\n");
    }

    return NO_ERROR;
}

status_t PQService::setPQDCIndex(int32_t level, int32_t index)
{
    Mutex::Autolock _l(mLock);

    if (m_drvID < 0)
    {
        PQ_LOGE("[PQ] open /proc/mtk_mira fail...");
        return UNKNOWN_ERROR;
    }

    ioctl(m_drvID, DISP_IOCTL_PQ_GET_DC_PARAM, &pqdcparam);

    pqdcparam.param[index] = level;

    ioctl(m_drvID, DISP_IOCTL_PQ_SET_DC_PARAM, &pqdcparam);

    return NO_ERROR;
}

status_t PQService::getPQDCIndex(DISP_PQ_DC_PARAM *dcparam, int32_t index)
 {
    Mutex::Autolock _l(mLock);

    if (m_drvID < 0)
    {
        PQ_LOGE("[PQ] open /proc/mtk_mira fail...");
        return UNKNOWN_ERROR ;
    }

    ioctl(m_drvID, DISP_IOCTL_PQ_GET_DC_PARAM, &pqdcparam);

    memcpy(dcparam, &pqdcparam, sizeof(pqdcparam));

    return NO_ERROR;
}
status_t PQService::getColorCapInfo(MDP_COLOR_CAP *param)
 {
    Mutex::Autolock _l(mLock);

    if (m_drvID < 0)
    {
        PQ_LOGE("[PQ] open /proc/mtk_mira fail...");
        return UNKNOWN_ERROR ;
    }

    ioctl(m_drvID, DISP_IOCTL_PQ_GET_MDP_COLOR_CAP, param);


    return NO_ERROR;
}

status_t PQService::getTDSHPReg(MDP_TDSHP_REG_EX *param)
 {
    Mutex::Autolock _l(mLock);

    if (m_drvID < 0)
    {
        PQ_LOGE("[PQ] open /proc/mtk_mira fail...");
        return UNKNOWN_ERROR ;
    }

    memcpy(param, &g_tdshp_reg, sizeof(g_tdshp_reg));

    return NO_ERROR;
}

status_t PQService::getPQDSIndex(DISP_PQ_DS_PARAM_EX *dsparam)
 {
    Mutex::Autolock _l(mLock);

    if (m_drvID < 0)
    {
        PQ_LOGE("[PQ] open /proc/mtk_mira fail...");
        return UNKNOWN_ERROR ;
    }

    memcpy(dsparam, &g_PQ_DS_Param, sizeof(g_PQ_DS_Param));

    return NO_ERROR;
}
status_t PQService::getColorIndex(DISP_PQ_PARAM *index, int32_t scenario, int32_t mode)
{
    Mutex::Autolock _l(mLock);
    // the statements in #ifndef DISP_COLOR_OFF are used when DISP only

    PQScenario = scenario;
    int percentage;
    //scenario will be passed from MDP, return corresponding param of scenario automatically

    // mdp_pqparam_standard/vivid will be added

    if (PQMode == PQ_PIC_MODE_STANDARD || PQMode == PQ_PIC_MODE_VIVID) {

        if (PQScenario ==  SCENARIO_PICTURE) {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 0], sizeof(DISP_PQ_PARAM));
        }
        else  if (PQScenario == SCENARIO_VIDEO) {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 1], sizeof(DISP_PQ_PARAM));
        }
        else  if (PQScenario == SCENARIO_ISP_PREVIEW) {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 2], sizeof(DISP_PQ_PARAM));
        }
        else {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 0], sizeof(DISP_PQ_PARAM));
            PQ_LOGD("[PQ_SERVICE] PQService : getColorIndex, invalid scenario\n");
        }
    }
    else if (PQMode == PQ_PIC_MODE_USER_DEF) {
        memcpy(index, &pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT], sizeof(DISP_PQ_PARAM));
    }
    else {
        memcpy(index, &pqparam_table[0], sizeof(DISP_PQ_PARAM));
        PQ_LOGD("[PQ_SERVICE] PQService : getColorIndex, invalid mode\n");
    }

    return NO_ERROR;
}

status_t PQService::getTDSHPIndex(DISP_PQ_PARAM *index, int32_t scenario, int32_t mode)
{
    Mutex::Autolock _l(mLock);
    PQScenario = scenario;
    //scenario will be passed from MDP, return corresponding param of scenario automatically
    // mdp_pqparam_standard/vivid will be added
    if (PQMode == PQ_PIC_MODE_STANDARD || PQMode == PQ_PIC_MODE_VIVID) {

        if (PQScenario ==  SCENARIO_PICTURE) {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 0], sizeof(DISP_PQ_PARAM));
        }
        else  if (PQScenario == SCENARIO_VIDEO) {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 1], sizeof(DISP_PQ_PARAM));
        }
        else  if (PQScenario == SCENARIO_ISP_PREVIEW) {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 2], sizeof(DISP_PQ_PARAM));
        }
        else {
            memcpy(index, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 0], sizeof(DISP_PQ_PARAM));
            PQ_LOGD("[PQ_SERVICE] PQService : getTDSHPIndex, invalid scenario\n");
        }
    }
    else if (PQMode == PQ_PIC_MODE_USER_DEF) {
        memcpy(index, &pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT], sizeof(DISP_PQ_PARAM));
    }
    else {
        memcpy(index, &pqparam_table[0], sizeof(DISP_PQ_PARAM));
        PQ_LOGD("[PQ_SERVICE] PQService : getTDSHPIndex, invalid mode\n");
    }

    return NO_ERROR;
}
status_t PQService::getTDSHPFlag(int32_t *TDSHPFlag)
{
    Mutex::Autolock _l(mLock);

    ioctl(m_drvID, DISP_IOCTL_PQ_GET_TDSHP_FLAG, TDSHPFlag);
    PQ_LOGD("[PQ_SERVICE] DISP_IOCTL_PQ_GET_TDSHP_FLAG()... tuning flag[%d]", *TDSHPFlag);

    return NO_ERROR;
}

status_t PQService::setTDSHPFlag(int32_t TDSHPFlag)
{
    Mutex::Autolock _l(mLock);

    if (m_drvID < 0)
    {
        PQ_LOGE("[PQ] setTDSHPFlag(), open dev fail...");
    }

    PQ_LOGD("[PQ_SERVICE] setTuningFlag[%d]", TDSHPFlag);

    unsigned int *base = (unsigned int *) mAshMemHeap->getBase();
    *(base + ASHMEM_TUNING_FLAG) = TDSHPFlag;

    PQ_LOGD("[PQ_CMD] Set TuningFlag in AshmemPtr : %x\n",*(base + ASHMEM_TUNING_FLAG));

    return NO_ERROR;
}


bool PQService::getCCorrCoefByIndex(int32_t coefTableIdx, uint32_t coef[3][3])
{
    unsigned int coef_sum = 0;

    PQ_LOGD("ccorr table index=%d", coefTableIdx);

    if (mCcorrDebug == true) { /* scenario debug mode */
        for (int y = 0; y < 3; y += 1) {
            for (int x = 0; x < 3; x += 1) {
                coef[y][x] = 0;
            }
        }
        int index = PQMode;
        if (PQScenario ==  SCENARIO_PICTURE)
            index += 0;
        else if (PQScenario ==  SCENARIO_VIDEO)
            index += 1;
        else
            index += 2;
        index = index % 3;
        coef[index][index] = 1024;
        coef_sum = 1024;
    } else { /* normal mode */
        for (int y = 0; y < 3; y += 1) {
            for (int x = 0; x < 3; x += 1) {
                coef[y][x] = pqindex.CCORR_COEF[coefTableIdx][y][x];
                coef_sum += coef[y][x];
            }
        }
    }

    if (coef_sum == 0) { /* zero coef protect */
        coef[0][0] = 1024;
        coef[1][1] = 1024;
        coef[2][2] = 1024;
        PQ_LOGD("ccorr coef all zero, prot on");
    }

    return true;
}


bool PQService::configCCorrCoef(int drvID, uint32_t coef[3][3])
{
    DISP_CCORR_COEF_T ccorr;

    ccorr.hw_id = DISP_CCORR0;
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            ccorr.coef[y][x] = coef[y][x];
        }
    }

    int ret = -1;
    ret = ioctl(drvID, DISP_IOCTL_SET_CCORR, &ccorr);
    if (ret == -1)
        PQ_LOGD("ccorr ioctl fail");

    return (ret == 0);
}


status_t PQService::configCCorrCoefByIndex(int32_t coefTableIdx, int32_t drvID)
{
#ifndef CCORR_OFF
    uint32_t coef[3][3];

    if (getCCorrCoefByIndex(coefTableIdx, coef))
        configCCorrCoef(drvID, coef);
#endif

    return NO_ERROR;
}


 status_t  PQService::setDISPScenario(int32_t scenario)
{
    Mutex::Autolock _l(mLock);
    PQScenario = scenario;
#ifndef DISP_COLOR_OFF
    int percentage = 0;

    if (m_drvID < 0)
        return UNKNOWN_ERROR;

    PQ_LOGD("[PQ_SERVICE] PQService : PQScenario = %d, PQMode = %d\n",PQScenario,PQMode);
    // mdp_pqparam_standard/vivid will be added

    if (PQMode == PQ_PIC_MODE_STANDARD || PQMode == PQ_PIC_MODE_VIVID) {

        if (PQScenario ==  SCENARIO_PICTURE) {
            memcpy(&pqparam, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 0], sizeof(DISP_PQ_PARAM));
        }
        else  if (PQScenario == SCENARIO_VIDEO) {
            memcpy(&pqparam, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 1], sizeof(DISP_PQ_PARAM));
        }
        else  if (PQScenario == SCENARIO_ISP_PREVIEW) {
            memcpy(&pqparam, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 2], sizeof(DISP_PQ_PARAM));
        }
        else {
            memcpy(&pqparam, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 0], sizeof(DISP_PQ_PARAM));
            PQ_LOGD("[PQ_SERVICE] PQService : setDISPScenario, invalid scenario\n");
        }
    }
    else if (PQMode == PQ_PIC_MODE_USER_DEF) {
        if (PQScenario ==  SCENARIO_PICTURE) {
            percentage = pqparam_mapping.image;
        }
        else  if (PQScenario == SCENARIO_VIDEO) {
            percentage = pqparam_mapping.video;
        }
        else  if (PQScenario == SCENARIO_ISP_PREVIEW) {
            percentage = pqparam_mapping.camera;
        }
        else {
            percentage = pqparam_mapping.image;
            PQ_LOGD("[PQ_SERVICE] PQService : setDISPScenario, invalid scenario\n");
        }

        calcPQStrength(&pqparam, &pqparam, percentage);
    }
    else {
        memcpy(&pqparam, &pqparam_table[0], sizeof(DISP_PQ_PARAM));
        PQ_LOGD("[PQ_SERVICE] PQService : setDISPScenario, invalid mode\n");
    }

    setPQParamWithFilter(m_drvID, &pqparam);
#endif

    return NO_ERROR;
}



status_t PQService::setFeatureSwitch(IPQService::PQFeatureID id, uint32_t value)
{
    Mutex::Autolock _l(mLock);

    uint32_t featureID = static_cast<PQFeatureID>(id);
    status_t ret = NO_ERROR;


    PQ_LOGD("[PQ_SERVICE] setFeatureSwitch(), feature[%d], value[%d]", featureID, value);

    switch (featureID) {
    case DISPLAY_COLOR:
        enableDisplayColor(value);
        break;
    case CONTENT_COLOR:
        // not implemented
        PQ_LOGE("[PQ_SERVICE] setFeatureSwitch(), CONTENT_COLOR not implemented!!");
        ret = UNKNOWN_ERROR;
        break;
    case CONTENT_COLOR_VIDEO:
        enableContentColorVideo(value);
        break;
    case SHARPNESS:
        enableSharpness(value);
        break;
    case DYNAMIC_CONTRAST:
        enableDynamicContrast(value);
        break;
    case DYNAMIC_SHARPNESS:
        enableDynamicSharpness(value);
        break;
    case DISPLAY_GAMMA:
        enableDisplayGamma(value);
        break;
    case DISPLAY_OVER_DRIVE:
        enableDisplayOverDrive(value);
        break;

    default:
        PQ_LOGE("[PQ_SERVICE] setFeatureSwitch(), feature[%d] is not implemented!!", id);
        ret = UNKNOWN_ERROR;
        break;
    }
    return ret;
}

status_t PQService::getFeatureSwitch(IPQService::PQFeatureID id, uint32_t *value)
{
    Mutex::Autolock _l(mLock);

    uint32_t featureID = static_cast<PQFeatureID>(id);

    if (featureID < PQ_FEATURE_MAX)
    {
        *value = m_bFeatureSwitch[featureID];
        PQ_LOGD("[PQ_SERVICE] getFeatureSwitch(), feature[%d], value[%d]", featureID, *value);

        return NO_ERROR;
    }
    else
    {
        *value = 0;
        PQ_LOGE("[PQ_SERVICE] getFeatureSwitch(), unsupported feature[%d]", featureID);

        return UNKNOWN_ERROR;
    }
}

status_t PQService::enableDisplayColor(uint32_t value)
{
#ifndef DISP_COLOR_OFF
    int bypass;
    PQ_LOGD("[PQ_SERVICE] enableDisplayColor(), enable[%d]", value);

    if (m_drvID < 0)
    {
        PQ_LOGE("[PQ_SERVICE] open device fail!!");
        return UNKNOWN_ERROR ;

    }

    //  set bypass COLOR to disp driver.
    if (value > 0)
    {
        bypass = 0;
        ioctl(m_drvID, DISP_IOCTL_PQ_SET_BYPASS_COLOR, &bypass);
    }
    else
    {
        bypass = 1;
        ioctl(m_drvID, DISP_IOCTL_PQ_SET_BYPASS_COLOR, &bypass);
    }
    PQ_LOGD("[PQService] enableDisplayColor[%d]", value);

    m_bFeatureSwitch[DISPLAY_COLOR] = value;
#endif

    return NO_ERROR;
}

status_t PQService::enableContentColorVideo(uint32_t value)
{
#ifdef MDP_COLOR_ENABLE
    char pvalue[PROPERTY_VALUE_MAX];
    int ret;

    snprintf(pvalue, PROPERTY_VALUE_MAX, "%d\n", value);
    ret = property_set(PQ_MDP_COLOR_EN_STR  , pvalue);
    PQ_LOGD("[PQService] enableContentColorVideo[%d]", value);

    m_bFeatureSwitch[CONTENT_COLOR_VIDEO] = value;
#endif

    return NO_ERROR;
}

status_t PQService::enableSharpness(uint32_t value)
{
    char pvalue[PROPERTY_VALUE_MAX];
    int ret;

    snprintf(pvalue, PROPERTY_VALUE_MAX, "%d\n", value);
    ret = property_set(PQ_DBG_SHP_EN_STR, pvalue);
    PQ_LOGD("[PQService] enableSharpness[%d]", value);

    m_bFeatureSwitch[SHARPNESS] = value;

    return NO_ERROR;
}

status_t PQService::enableDynamicContrast(uint32_t value)
{
    char pvalue[PROPERTY_VALUE_MAX];
    int ret;

    snprintf(pvalue, PROPERTY_VALUE_MAX, "%d\n", value);
    ret = property_set(PQ_ADL_PROPERTY_STR, pvalue);
    PQ_LOGD("[PQService] enableDynamicContrast[%d]", value);

    m_bFeatureSwitch[DYNAMIC_CONTRAST] = value;

    return NO_ERROR;
}

status_t PQService::enableDynamicSharpness(uint32_t value)
{
    char pvalue[PROPERTY_VALUE_MAX];
    int ret;

    snprintf(pvalue, PROPERTY_VALUE_MAX, "%d\n", value);
    ret = property_set(PQ_DBG_DSHP_EN_STR, pvalue);
    PQ_LOGD("[PQService] enableDynamicSharpness[%d]", value);

    m_bFeatureSwitch[DYNAMIC_CONTRAST] = value;

    return NO_ERROR;
}

status_t PQService::enableDisplayGamma(uint32_t value)
{
    if (value > 0)
    {
        char pvalue[PROPERTY_VALUE_MAX];
        char dvalue[PROPERTY_VALUE_MAX];
        int index;

        snprintf(dvalue, PROPERTY_VALUE_MAX, "%d\n", GAMMA_INDEX_DEFAULT);
        property_get(GAMMA_INDEX_PROPERTY_NAME, pvalue, dvalue);
        index = atoi(pvalue);
        _setGammaIndex(index);
    }
    else
    {
        _setGammaIndex(GAMMA_INDEX_DEFAULT);
    }
    PQ_LOGD("[PQService] enableDisplayGamma[%d]", value);

    m_bFeatureSwitch[DISPLAY_GAMMA] = value;

    return NO_ERROR;
}

status_t PQService::enableDisplayOverDrive(uint32_t value)
{
    DISP_OD_CMD cmd;

    if (m_drvID < 0)
    {
        PQ_LOGE("[PQService] enableDisplayOverDrive(), open device fail!!");
        return UNKNOWN_ERROR;
    }

    memset(&cmd, 0, sizeof(cmd));
    cmd.size = sizeof(cmd);
    cmd.type = 2;

    if (value > 0)
    {
        cmd.param0 = 1;
    }
    else
    {
        cmd.param0 = 0;
    }

    ioctl(m_drvID, DISP_IOCTL_OD_CTL, &cmd);

    PQ_LOGD("[PQService] enableDisplayOverDrive[%d]", value);

    m_bFeatureSwitch[DISPLAY_OVER_DRIVE] = value;

    return NO_ERROR;
}

status_t PQService::enableBlueLight(bool enable)
{
    Mutex::Autolock _l(mLock);

    PQ_LOGD("[PQ_SERVICE] PQService : enableBlueLight(%d)", (enable ? 1 : 0));

    blueLight->setEnabled(enable);

    refreshDisplay();

    return NO_ERROR;
}


status_t PQService::getBlueLightEnabled(bool *isEnabled)
{
    Mutex::Autolock _l(mLock);

    *isEnabled = blueLight->isEnabled();

    return NO_ERROR;
}


status_t PQService::setBlueLightStrength(int32_t strength)
{
    Mutex::Autolock _l(mLock);
    char value[PROPERTY_VALUE_MAX];

    PQ_LOGD("[PQ_SERVICE] PQService : setBlueLightStrength(%d)", strength);

    snprintf(value, PROPERTY_VALUE_MAX, "%d", strength);
    property_set(MTK_BLUELIGHT_STRENGTH_PROPERTY_NAME, value);

    blueLight->setStrength(strength);

    refreshDisplay();

    return NO_ERROR;
}


status_t PQService::getBlueLightStrength(int32_t *strength)
{
    char value[PROPERTY_VALUE_MAX];
    Mutex::Autolock _l(mLock);

    property_get(MTK_BLUELIGHT_STRENGTH_PROPERTY_NAME, value, "128");
    *strength = atoi(value);

    return NO_ERROR;
}


void PQService::initBlueLight()
{
    Mutex::Autolock _l(mLock);

    BluLightInitParam initParam;
    initParam.reserved = 0;
    blueLight->onInitCommon(initParam);
    blueLight->onInitPlatform(initParam);

    blueLight->setEnabled(false);
    mBlueLightDebugFlag = 0;

    char property[PROPERTY_VALUE_MAX];
    if (property_get(MTK_BLUELIGHT_STRENGTH_PROPERTY_NAME, property, NULL) > 0) {
        int strength = (int)strtoul(property, NULL, 0);
        PQ_LOGD("[PQ_SERVICE] Blue-light init strength: %d", strength);
        blueLight->setStrength(strength);
    }
}


// Compose ColorRegisters by current scenario setting
bool PQService::composeColorRegisters(void *_colorReg, const DISP_PQ_PARAM *pqparam)
{
    bool result = false;
    ColorRegisters *colorReg = static_cast<ColorRegisters*>(_colorReg);
    const DISPLAY_PQ_T *pqTable = &pqindex;

    colorReg->GLOBAL_SAT = pqTable->GLOBAL_SAT[pqparam->u4SatGain];
    colorReg->CONTRAST = pqTable->CONTRAST[pqparam->u4Contrast];
    colorReg->BRIGHTNESS = pqTable->BRIGHTNESS[pqparam->u4Brightness];
    getCCorrCoefByIndex(pqparam->u4Ccorr, colorReg->CCORR_COEF);
    if (sizeof(colorReg->PARTIAL_Y) == sizeof(pqTable->PARTIAL_Y[0]) &&
        sizeof(colorReg->PURP_TONE_S) == sizeof(pqTable->PURP_TONE_S[0]) &&
        sizeof(colorReg->SKIN_TONE_S) == sizeof(pqTable->SKIN_TONE_S[0]) &&
        sizeof(colorReg->GRASS_TONE_S) == sizeof(pqTable->GRASS_TONE_S[0]) &&
        sizeof(colorReg->SKY_TONE_S) == sizeof(pqTable->SKY_TONE_S[0]) &&
        sizeof(colorReg->PURP_TONE_H) == sizeof(pqTable->PURP_TONE_H[0]) &&
        sizeof(colorReg->SKIN_TONE_H) == sizeof(pqTable->SKIN_TONE_H[0]) &&
        sizeof(colorReg->GRASS_TONE_H) == sizeof(pqTable->GRASS_TONE_H[0]) &&
        sizeof(colorReg->SKY_TONE_H) == sizeof(pqTable->SKY_TONE_H[0]))
    {
        memcpy(colorReg->PARTIAL_Y, pqTable->PARTIAL_Y[pqparam->u4PartialY], sizeof(colorReg->PARTIAL_Y));
        memcpy(colorReg->PURP_TONE_S, pqTable->PURP_TONE_S[pqparam->u4SatAdj[PURP_TONE]], sizeof(colorReg->PURP_TONE_S));
        memcpy(colorReg->SKIN_TONE_S, pqTable->SKIN_TONE_S[pqparam->u4SatAdj[SKIN_TONE]], sizeof(colorReg->SKIN_TONE_S));
        memcpy(colorReg->GRASS_TONE_S, pqTable->GRASS_TONE_S[pqparam->u4SatAdj[GRASS_TONE]], sizeof(colorReg->GRASS_TONE_S));
        memcpy(colorReg->SKY_TONE_S, pqTable->SKY_TONE_S[pqparam->u4SatAdj[SKY_TONE]], sizeof(colorReg->SKY_TONE_S));
        memcpy(colorReg->PURP_TONE_H, pqTable->PURP_TONE_H[pqparam->u4HueAdj[PURP_TONE]], sizeof(colorReg->PURP_TONE_H));
        memcpy(colorReg->SKIN_TONE_H, pqTable->SKIN_TONE_H[pqparam->u4HueAdj[SKIN_TONE]], sizeof(colorReg->SKIN_TONE_H));
        memcpy(colorReg->GRASS_TONE_H, pqTable->GRASS_TONE_H[pqparam->u4HueAdj[GRASS_TONE]], sizeof(colorReg->GRASS_TONE_H));
        memcpy(colorReg->SKY_TONE_H, pqTable->SKY_TONE_H[pqparam->u4HueAdj[SKY_TONE]], sizeof(colorReg->SKY_TONE_H));

        result = true;
    } else {
        PQ_LOGE("composeColorRegisters: Parameter size does not match (%d, %d) (%d, %d) (%d, %d)",
            sizeof(colorReg->PARTIAL_Y), sizeof(pqTable->PARTIAL_Y[0]),
            sizeof(colorReg->PURP_TONE_S), sizeof(pqTable->PURP_TONE_S[0]),
            sizeof(colorReg->SKIN_TONE_S), sizeof(pqTable->SKIN_TONE_S[0]));
        PQ_LOGE("composeColorRegisters: (%d, %d) (%d, %d) (%d, %d) (%d, %d) (%d, %d) (%d, %d)",
            sizeof(colorReg->GRASS_TONE_S), sizeof(pqTable->GRASS_TONE_S[0]),
            sizeof(colorReg->SKY_TONE_S), sizeof(pqTable->SKY_TONE_S[0]),
            sizeof(colorReg->PURP_TONE_H), sizeof(pqTable->PURP_TONE_H[0]),
            sizeof(colorReg->SKIN_TONE_H), sizeof(pqTable->SKIN_TONE_H[0]),
            sizeof(colorReg->GRASS_TONE_H), sizeof(pqTable->GRASS_TONE_H[0]),
            sizeof(colorReg->SKY_TONE_H), sizeof(pqTable->SKY_TONE_H[0]));
    }

    return result;
}


// Translate ColorRegisters to DISPLAY_COLOR_REG_T
bool PQService::translateColorRegisters(void *algoReg, DISPLAY_COLOR_REG_T *drvReg)
{
    size_t algoSize = offsetof(ColorRegisters, SKY_TONE_H) + sizeof(((ColorRegisters*)0)->SKY_TONE_H);
    size_t drvSize = offsetof(DISPLAY_COLOR_REG_T, SKY_TONE_H) + sizeof(drvReg->SKY_TONE_H);

    if (algoSize != drvSize) {
        PQ_LOGE("translateColorRegisters: Structure size changed SW:%u != DRV:%u",
            algoSize, drvSize);
        return false;
    }

    memcpy(drvReg, algoReg, sizeof(DISPLAY_COLOR_REG_T));

    return true;
}


bool PQService::setPQParamWithFilter(int drvID, const DISP_PQ_PARAM *pqparam)
{
    int ret = -1;

#ifdef MTK_BLULIGHT_DEFENDER_SUPPORT
    DISPLAY_COLOR_REG_T *drvReg = new DISPLAY_COLOR_REG_T;

    if (blueLight->isEnabled()) {
        ColorRegisters *inReg = new ColorRegisters;
        ColorRegisters *outReg = new ColorRegisters;

        if (composeColorRegisters(inReg, pqparam)) {
            dumpColorRegisters("Blue-light input:", inReg);
            blueLight->onCalculate(*inReg, outReg);
            dumpColorRegisters("Blue-light output:", outReg);

            if (translateColorRegisters(outReg, drvReg)) {
                ret = ioctl(drvID, DISP_IOCTL_SET_COLOR_REG, drvReg);
                configCCorrCoef(drvID, outReg->CCORR_COEF);
            }
        }

        delete inReg;
        delete outReg;
    } else {
        ColorRegisters *colorReg = new ColorRegisters;

        if (composeColorRegisters(colorReg, pqparam)) {
            if (translateColorRegisters(colorReg, drvReg)) {
                ret = ioctl(drvID, DISP_IOCTL_SET_COLOR_REG, drvReg);
                configCCorrCoef(drvID, colorReg->CCORR_COEF);
            }
        }

        delete colorReg;
    }

    delete drvReg;

    if (ret != 0) {
        PQ_LOGE("[PQ_SERVICE] setPQParamWithFilter: DISP_IOCTL_SET_COLOR_REG: ret = %d", ret);
    }

#else

    ret = ioctl(drvID, DISP_IOCTL_SET_PQPARAM, pqparam);
    PQ_LOGD("[PQ_SERVICE] setPQParamWithFilter: DISP_IOCTL_SET_PQPARAM: ret = %d", ret);
    configCCorrCoefByIndex(pqparam->u4Ccorr, drvID);

#endif

    return (ret == 0);
}


status_t PQService::setTuningField(int32_t module, int32_t field, int32_t value)
{
    switch (module) {
    case IPQService::MOD_DISPLAY:
        if (field == 0 && value == 0) {
            refreshDisplay();
            return NO_ERROR;
        }
        break;
    case IPQService::MOD_BLUE_LIGHT:
        if (blueLight->setTuningField(field, static_cast<unsigned int>(value)))
            return NO_ERROR;
        break;
    case IPQService::MOD_DYNAMIC_SHARPNESS:
        g_PQ_DS_Param.param[field] = static_cast<unsigned int>(value);
        PQ_LOGD("[PQ_SERVICE] PQService : write module 0x%x, field 0x%x = 0x%x\n", module, field, value);
        return NO_ERROR;
        break;
    case IPQService::MOD_TDSHP_REG:
        g_tdshp_reg.param[field] = static_cast<unsigned int>(value);
        PQ_LOGD("[PQ_SERVICE] PQService : write module 0x%x, field 0x%x = 0x%x\n", module, field, value);
        return NO_ERROR;
        break;
    case IPQService::MOD_ULTRARESOLUTION:
        if (field >= RSZ_tableMode && field < RSZ_INDEX_MAX) {
            rszparam.param[field] = static_cast<unsigned int>(value);
            PQ_LOGD("[PQ_SERVICE] PQService : write module 0x%x, field 0x%x = 0x%x\n", module, field, value);
            return NO_ERROR;
        }
        break;
    }
    return BAD_INDEX;
}


status_t PQService::getTuningField(int32_t module, int32_t field, int32_t *value)
{
    unsigned int uvalue;

    switch (module) {
    case IPQService::MOD_DISPLAY:
        break;
    case IPQService::MOD_BLUE_LIGHT:
        if (blueLight->getTuningField(field, &uvalue)) {
            *value = static_cast<int32_t>(uvalue);
            return NO_ERROR;
        }
        break;
    case IPQService::MOD_DYNAMIC_SHARPNESS:
        *value = (unsigned int)g_PQ_DS_Param.param[field];
        PQ_LOGD("[PQ_SERVICE] PQService : read module 0x%x, field 0x%x = 0x%x\n", module, field, *value);
        return NO_ERROR;
        break;
    case IPQService::MOD_TDSHP_REG:
        *value = (unsigned int)g_tdshp_reg.param[field];
        PQ_LOGD("[PQ_SERVICE] PQService : read module 0x%x, field 0x%x = 0x%x\n", module, field, *value);
        return NO_ERROR;
        break;
    case IPQService::MOD_ULTRARESOLUTION:
        if (field >= RSZ_tableMode && field < RSZ_INDEX_MAX) {
            *value = (unsigned int)rszparam.param[field];
            PQ_LOGD("[PQ_SERVICE] PQService : read module 0x%x, field 0x%x = 0x%x\n", module, field, *value);
            return NO_ERROR;
        }
        break;
    }

    *value = 0;
    return BAD_INDEX;
}


void PQService::refreshDisplay(void)
{
    setPQParamWithFilter(m_drvID, &pqparam);
}


void PQService::dumpColorRegisters(const char *prompt, void *_colorReg)
{
    ColorRegisters *colorReg = static_cast<ColorRegisters*>(_colorReg);

    if (mBlueLightDebugFlag & 0x1) {
        static const int buf_size = 512;
        char *buffer = new char[buf_size];

        PQ_LOGD("[PQ_SERVICE] %s", prompt);
        PQ_LOGD("[PQ_SERVICE] ColorRegisters: Sat:%d, Con:%d, Bri: %d",
            colorReg->GLOBAL_SAT, colorReg->CONTRAST, colorReg->BRIGHTNESS);

        #define ARR_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

        PQ_LOGD("[PQ_SERVICE] PARTIAL_Y:%s",
            printIntArray(buffer, buf_size, &colorReg->PARTIAL_Y[0], ARR_LEN(colorReg->PARTIAL_Y)));

        for (int i = 0; i < CLR_PQ_PARTIALS_CONTROL; i++) {
            PQ_LOGD("[PQ_SERVICE] PURP_TONE_S[%d]:%s", i,
                printIntArray(buffer, buf_size, &colorReg->PURP_TONE_S[i][0], ARR_LEN(colorReg->PURP_TONE_S[0])));
            PQ_LOGD("[PQ_SERVICE] SKIN_TONE_S[%d]:%s", i,
                printIntArray(buffer, buf_size, &colorReg->SKIN_TONE_S[i][0], ARR_LEN(colorReg->SKIN_TONE_S[0])));
            PQ_LOGD("[PQ_SERVICE] GRASS_TONE_S[%d]:%s", i,
                printIntArray(buffer, buf_size, &colorReg->GRASS_TONE_S[i][0], ARR_LEN(colorReg->GRASS_TONE_S[0])));
            PQ_LOGD("[PQ_SERVICE] SKY_TONE_S[%d]:%s", i,
                printIntArray(buffer, buf_size, &colorReg->SKY_TONE_S[i][0], ARR_LEN(colorReg->SKY_TONE_S[0])));
        }

        PQ_LOGD("[PQ_SERVICE] PURP_TONE_H:%s",
            printIntArray(buffer, buf_size, &colorReg->PURP_TONE_H[0], ARR_LEN(colorReg->PURP_TONE_H)));
        PQ_LOGD("[PQ_SERVICE] SKIN_TONE_H:%s",
            printIntArray(buffer, buf_size, &colorReg->SKIN_TONE_H[0], ARR_LEN(colorReg->SKIN_TONE_H)));
        PQ_LOGD("[PQ_SERVICE] GRASS_TONE_H:%s",
            printIntArray(buffer, buf_size, &colorReg->GRASS_TONE_H[0], ARR_LEN(colorReg->GRASS_TONE_H)));
        PQ_LOGD("[PQ_SERVICE] SKY_TONE_H:%s",
            printIntArray(buffer, buf_size, &colorReg->SKY_TONE_H[0], ARR_LEN(colorReg->SKY_TONE_H)));
        PQ_LOGD("[PQ_SERVICE] CCORR_COEF:%s",
            printIntArray(buffer, buf_size, reinterpret_cast<unsigned int*>(colorReg->CCORR_COEF), 9));

        #undef ARR_LEN

        delete [] buffer;
    }
}

sp<IMemoryHeap> PQService::getAshmem(void)
{
    return mAshMemHeap;
}

void PQService::onFirstRef()
{
    run("PQServiceMain", PRIORITY_DISPLAY);
    PQ_LOGD("[PQ_SERVICE] PQService : onFirstRef");
}

status_t PQService::readyToRun()
{
    PQ_LOGD("[PQ_SERVICE] PQService is ready to run.");
    return NO_ERROR;
}


bool PQService::initDriverRegs()
{
    return NO_ERROR;
}

bool PQService::threadLoop()
{
    char value[PROPERTY_VALUE_MAX];
    int i;
    int32_t  status;
    int percentage = pqparam_mapping.image;  //default scenario = image
    MDP_TDSHP_FLAG = 0;

    property_get(PQ_PIC_MODE_PROPERTY_STR, value, PQ_PIC_MODE_DEFAULT);
    PQMode = atoi(value);
    PQScenario = SCENARIO_PICTURE;
    mCcorrDebug = false;

    mAshMemHeap = new MemoryHeapBase(MEMORY_SIZE);
    unsigned int *base = (unsigned int *) mAshMemHeap->getBase();
    memset(base, 0, MEMORY_SIZE);

    /*load */
    void    *handle;
    DISP_PQ_MAPPING_PARAM *pq_mapping_ptr;
    DISPLAY_PQ_T  *pq_table_ptr;
    DISPLAY_TDSHP_T *tdshp_table_ptr;

    /* open the needed object */
    handle = dlopen("libpq_cust.so", RTLD_LAZY);
    if (!handle) {
        PQ_LOGD("[PQ_SERVICE] can't open libpq_cust.so\n");
        goto PQCONFIG;
    }
    /* find the address of function and data objects */

    pq_mapping_ptr = (DISP_PQ_MAPPING_PARAM *)dlsym(handle, "pqparam_mapping");
    if (!pq_mapping_ptr) {
        PQ_LOGD("[PQ_SERVICE] pqparam_mapping is not found in libpq_cust.so\n");
        dlclose(handle);
        goto PQCONFIG;
    }
    memcpy(&pqparam_mapping, pq_mapping_ptr, sizeof(DISP_PQ_MAPPING_PARAM));

    if (!loadPqparamTable(&pqparam_table[0], handle)) {
        dlclose(handle);
        goto PQCONFIG;
    }


#ifdef MDP_COLOR_ENABLE
    pq_table_ptr = (DISPLAY_PQ_T *)dlsym(handle, "secondary_pqindex");
#else
    pq_table_ptr = (DISPLAY_PQ_T *)dlsym(handle, "primary_pqindex");
#endif

    if (!pq_table_ptr) {
        PQ_LOGD("[PQ_SERVICE] pqindex is not found in libpq_cust.so\n");
        dlclose(handle);
        goto PQCONFIG;
    }
    memcpy(&pqindex, pq_table_ptr, sizeof(DISPLAY_PQ_T));

    tdshp_table_ptr = (DISPLAY_TDSHP_T *)dlsym(handle, "tdshpindex");
    if (!tdshp_table_ptr) {
        PQ_LOGD("[PQ_SERVICE] tdshpindex is not found in libpq_cust.so\n");
        dlclose(handle);
        goto PQCONFIG;
    }
    memcpy(&tdshpindex, tdshp_table_ptr, sizeof(DISPLAY_TDSHP_T));

    {
        gamma_entry_t* ptr = (gamma_entry_t*)dlsym(handle, "cust_gamma");
        if (!ptr) {
            PQ_LOGD("[PQ_SERVICE] cust_gamma is not found in libpq_cust.so\n");
            dlclose(handle);
            goto PQCONFIG;
        }
        memcpy(&m_CustGamma[0][0], ptr, sizeof(gamma_entry_t) * GAMMA_LCM_MAX * GAMMA_INDEX_MAX);
        configGamma(PQMode);
    }
    dlclose(handle);

    initBlueLight();

PQCONFIG:

#ifndef DISP_COLOR_OFF

    PQ_LOGD("[PQ_SERVICE] DISP PQ init start...");

    if (m_drvID < 0)
    {
        PQ_LOGE("PQ device open failed!!");
    }

    // pq index
    ioctl(m_drvID, DISP_IOCTL_SET_PQINDEX, &pqindex);

    if (PQMode == PQ_PIC_MODE_STANDARD || PQMode == PQ_PIC_MODE_VIVID)
    {
        memcpy(&pqparam, &pqparam_table[(PQMode) * PQ_SCENARIO_COUNT + 0], sizeof(DISP_PQ_PARAM));

        property_get(PQ_TDSHP_PROPERTY_STR, value, PQ_TDSHP_STANDARD_DEFAULT);
        property_set(PQ_TDSHP_PROPERTY_STR, value);

        setPQParamWithFilter(m_drvID, &pqparam);
    }
    else if (PQMode == PQ_PIC_MODE_USER_DEF)
    {

        // base on vivid
        //memcpy(&pqparam_user_def, &pqparam_usr, sizeof(pqparam_usr));

        property_get(PQ_TDSHP_PROPERTY_STR, value, PQ_TDSHP_INDEX_DEFAULT);
        i = atoi(value);
        PQ_LOGD("[PQ_SERVICE] property get... tdshp[%d]", i);
        pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT].u4SHPGain = i;

        property_get(PQ_GSAT_PROPERTY_STR, value, PQ_GSAT_INDEX_DEFAULT);
        i = atoi(value);
        PQ_LOGD("[PQ_SERVICE] property get... gsat[%d]", i);
        pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT].u4SatGain = i;

        property_get(PQ_CONTRAST_PROPERTY_STR, value, PQ_CONTRAST_INDEX_DEFAULT);
        i = atoi(value);
        PQ_LOGD("[PQ_SERVICE] property get... contrast[%d]", i);
        pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT].u4Contrast = i;

        property_get(PQ_PIC_BRIGHT_PROPERTY_STR, value, PQ_PIC_BRIGHT_INDEX_DEFAULT);
        i = atoi(value);
        PQ_LOGD("[PQ_SERVICE] property get... pic bright[%d]", i);
        pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT].u4Brightness = i;

        calcPQStrength(&pqparam, &pqparam_table[PQ_PREDEFINED_MODE_COUNT * PQ_SCENARIO_COUNT], percentage);

        PQ_LOGD("[PQ_SERVICE] --Init_PQ_Userdef, gsat[%d], cont[%d], bri[%d] ", pqparam.u4SatGain, pqparam.u4Contrast, pqparam.u4Brightness);
        PQ_LOGD("[PQ_SERVICE] --Init_PQ_Userdef, hue0[%d], hue1[%d], hue2[%d], hue3[%d] ", pqparam.u4HueAdj[0], pqparam.u4HueAdj[1], pqparam.u4HueAdj[2], pqparam.u4HueAdj[3]);
        PQ_LOGD("[PQ_SERVICE] --Init_PQ_Userdef, sat0[%d], sat1[%d], sat2[%d], sat3[%d] ", pqparam.u4SatAdj[0], pqparam.u4SatAdj[1], pqparam.u4SatAdj[2], pqparam.u4SatAdj[3]);

        setPQParamWithFilter(m_drvID, &pqparam);
    }
    else
    {
        memcpy(&pqparam, &pqparam_table[0], sizeof(pqparam));

        PQ_LOGE("[PQ][main pq] main, property get... unknown pic_mode[%d]", PQMode);

        setPQParamWithFilter(m_drvID, &pqparam);
    }

    status = ioctl(m_drvID, DISP_IOCTL_SET_TDSHPINDEX, &tdshpindex);
    PQ_LOGD("[PQ_SERVICE] DISP_IOCTL_SET_TDSHPINDEX %d...",status);
    // write DC property
    property_get(PQ_ADL_PROPERTY_STR, value, PQ_ADL_INDEX_DEFAULT);
    property_set(PQ_ADL_PROPERTY_STR, value);

    PQ_LOGD("[PQ_SERVICE] DISP PQ init end...");

#else // DISP_COLOR_OFF

#ifdef MTK_BLULIGHT_DEFENDER_SUPPORT
    // We need a default pqparam
    memcpy(&pqparam, &pqparam_table[0], sizeof(DISP_PQ_PARAM));
    setPQParamWithFilter(m_drvID, &pqparam);
#endif // MTK_BLULIGHT_DEFENDER_SUPPORT

#endif // DISP_COLOR_OFF

    PQ_LOGD("[PQ_SERVICE] threadLoop config User_Def PQ... end");


    while(1)
    {
        sleep(10);
        //PQ_LOGD("[PQ_SERVICE] PQService threadloop");

    }

return true;
}

int PQService::_getLcmIndexOfGamma()
{
    static int lcmIdx = -1;

    if (lcmIdx == -1) {
        int ret = ioctl(m_drvID, DISP_IOCTL_GET_LCMINDEX, &lcmIdx);
        if (ret == 0) {
            if (lcmIdx < 0 || GAMMA_LCM_MAX <= lcmIdx) {
                PQ_LOGE("Invalid LCM index %d, GAMMA_LCM_MAX = %d", lcmIdx, GAMMA_LCM_MAX);
                lcmIdx = 0;
            }
        } else {
            PQ_LOGE("ioctl(DISP_IOCTL_GET_LCMINDEX) return %d", ret);
            lcmIdx = 0;
        }
    }

    PQ_LOGD("LCM index: %d/%d", lcmIdx, GAMMA_LCM_MAX);

    return lcmIdx;
}


void PQService::_setGammaIndex(int index)
{
    if (index < 0 || GAMMA_INDEX_MAX <= index)
        index = GAMMA_INDEX_DEFAULT;

    DISP_GAMMA_LUT_T *driver_gamma = new DISP_GAMMA_LUT_T;

    int lcm_id = _getLcmIndexOfGamma();

    const gamma_entry_t *entry = &(m_CustGamma[lcm_id][index]);
    driver_gamma->hw_id = DISP_GAMMA0;
    for (int i = 0; i < DISP_GAMMA_LUT_SIZE; i++) {
        driver_gamma->lut[i] = GAMMA_ENTRY((*entry)[0][i], (*entry)[1][i], (*entry)[2][i]);
    }

    ioctl(m_drvID, DISP_IOCTL_SET_GAMMALUT, driver_gamma);

    delete driver_gamma;
}

void PQService::configGamma(int picMode)
{
#if (GAMMA_LCM_MAX > 0) && (GAMMA_INDEX_MAX > 0)
    int lcmIndex = 0;
    int gammaIndex = 0;
#endif

#if GAMMA_LCM_MAX > 1
    lcmIndex = _getLcmIndexOfGamma();
#endif

#if GAMMA_INDEX_MAX > 1
    // get gamma index from runtime property configuration
    char property[PROPERTY_VALUE_MAX];

    gammaIndex = GAMMA_INDEX_DEFAULT;
    if (picMode == PQ_PIC_MODE_USER_DEF &&
            property_get(GAMMA_INDEX_PROPERTY_NAME, property, NULL) > 0 &&
            strlen(property) > 0)
    {
        gammaIndex = atoi(property);
    }

    if (gammaIndex < 0 || GAMMA_INDEX_MAX <= gammaIndex)
        gammaIndex = GAMMA_INDEX_DEFAULT;

    PQ_LOGD("Gamma index: %d/%d", gammaIndex, GAMMA_INDEX_MAX);
#endif

#if (GAMMA_LCM_MAX > 0) && (GAMMA_INDEX_MAX > 0)
    DISP_GAMMA_LUT_T *driverGamma = new DISP_GAMMA_LUT_T;

    const gamma_entry_t *entry = &(m_CustGamma[lcmIndex][gammaIndex]);
    driverGamma->hw_id = DISP_GAMMA0;
    for (int i = 0; i < DISP_GAMMA_LUT_SIZE; i++) {
        driverGamma->lut[i] = GAMMA_ENTRY((*entry)[0][i], (*entry)[1][i], (*entry)[2][i]);
    }

    ioctl(m_drvID, DISP_IOCTL_SET_GAMMALUT, driverGamma);

    delete driverGamma;
#endif
}

bool PQService::loadPqparamTable(DISP_PQ_PARAM *pqparam, void *handle)
{
    DISP_PQ_PARAM *pq_param_ptr;
    /* find the address of function and data objects */
    pq_param_ptr = (DISP_PQ_PARAM *)dlsym(handle, "pqparam_table");
    if (!pq_param_ptr) {
        PQ_LOGD("[PQ_SERVICE] pqparam_table is not found in libpq_cust.so\n");
        return false;
    }
    memcpy(pqparam, pq_param_ptr, sizeof(DISP_PQ_PARAM)*PQ_PARAM_TABLE_SIZE);

    return true;
}

void PQService::setDebuggingPqparam()
{
    if (mPQDebugFlag == 1) {
        pqparam_table[0].u4SHPGain = 2;
        pqparam_table[0].u4Brightness = 4;
        pqparam_table[1].u4SHPGain = 1;
        pqparam_table[1].u4Brightness = 6;
        pqparam_table[2].u4SHPGain = 0;
        pqparam_table[2].u4Brightness= 8;
    }
    else if (mPQDebugFlag == 0) {
        void    *handle;

        /* open the needed object */
        handle = dlopen("libpq_cust.so", RTLD_LAZY);
        if (!handle) {
            PQ_LOGD("[PQ_SERVICE] can't open libpq_cust.so\n");
            return;
        }
        loadPqparamTable(&pqparam_table[0], handle);
        dlclose(handle);
    }
}

static const String16 sDump("android.permission.DUMP");
status_t PQService::dump(int fd, const Vector<String16>& args)
{
    static const size_t SIZE = 4096;
    char *buffer;
    String8 result;

    buffer = new char[SIZE];
    buffer[0] = '\0';

    PQ_LOGD("[PQ_SERVICE] PQService dump");
    if (!PermissionCache::checkCallingPermission(sDump)) {
        snprintf(buffer, SIZE, "Permission Denial: "
                "can't dump SurfaceFlinger from pid=%d, uid=%d\n",
                IPCThreadState::self()->getCallingPid(),
                IPCThreadState::self()->getCallingUid());
        result.append(buffer);
    } else {
        // Try to get the main lock, but don't insist if we can't
        // (this would indicate AALService is stuck, but we want to be able to
        // print something in dumpsys).
        int retry = 3;
        while (mLock.tryLock() < 0 && --retry >= 0) {
            usleep(500 * 1000);
        }
        const bool locked(retry >= 0);
        if (!locked) {
            snprintf(buffer, SIZE,
                    "PQService appears to be unresponsive, "
                    "dumping anyways (no locks held)\n");
            result.append(buffer);
        } else {
            size_t numArgs = args.size();
            for (size_t argIndex = 0; argIndex < numArgs; ) {
                if (args[argIndex] == String16("--ccorrdebug")) {
                    mCcorrDebug = true;
                    snprintf(buffer, SIZE, "CCORR Debug On");
                } else if (args[argIndex] == String16("--ccorrnormal")) {
                    mCcorrDebug = false;
                    snprintf(buffer, SIZE, "CCORR Debug Off");
                } else if (args[argIndex] == String16("--bl")) {
                    if (argIndex + 1 < numArgs) {
                        bool enabled = blueLight->isEnabled();
                        argIndex++;
                        if (args[argIndex] == String16("on") || args[argIndex] == String16("1"))
                            enabled = true;
                        else
                            enabled = false;
                        blueLight->setEnabled(enabled);
                        snprintf(buffer, SIZE, "Blue light: %s\n", (enabled ? "on" : "off"));
                        result.append(buffer);
                        snprintf(buffer, SIZE, "Strength: %d\n", blueLight->getStrength());
                        refreshDisplay();
                    } else {
                        snprintf(buffer, SIZE, "on/off?\n");
                    }
                } else if (args[argIndex] == String16("--bl_debug")) {
                    if (argIndex + 1 < numArgs) {
                        argIndex++;
                        mBlueLightDebugFlag = asInt(args[argIndex]);
                        blueLight->setDebugFlag(mBlueLightDebugFlag);
                        snprintf(buffer, SIZE, "Debug level = 0x%x\n", mBlueLightDebugFlag);
                    }
                } else if (args[argIndex] == String16("--pqparam_debug_on")) {
                        mPQDebugFlag = 1;
                        setDebuggingPqparam();
                        snprintf(buffer, SIZE, "pqparam_debug On\n");
                } else if (args[argIndex] == String16("--pqparam_debug_off")) {
                        mPQDebugFlag = 0;
                        setDebuggingPqparam();
                        snprintf(buffer, SIZE, "pqparam_debug Off\n");
                }
                argIndex += 1;
            }
            result.append(buffer);
        }

        if (locked) {
            mLock.unlock();
        }
    }

    write(fd, result.string(), result.size());

    delete [] buffer;

    return NO_ERROR;
}

};
