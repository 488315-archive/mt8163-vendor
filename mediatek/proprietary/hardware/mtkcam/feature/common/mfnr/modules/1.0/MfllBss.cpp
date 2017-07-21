#include "MfllBss.h"
#include "MfllLog.h"

#include <libbss/MTKBss.h>

#include <camera_custom_mfll.h>

#define LOG_TAG "MfllCore/Bss"

using std::vector;
using namespace mfll;

IMfllBss* IMfllBss::createInstance()
{
    return reinterpret_cast<IMfllBss*>(new MfllBss);
}

void IMfllBss::destroyInstance()
{
    decStrong((void*)this);
}

MfllBss::MfllBss()
: m_roiPercetange(MFLL_BSS_ROI_PERCENTAGE)
{
}

vector<int> MfllBss::bss(
        const vector< sp<IMfllImageBuffer> >    &imgs,
        const vector<MfllMotionVector_t>        &mvs
        )
{
    mfllAutoLogFunc();
    mfllAutoTraceFunc();

    MTKBss *mtkBss = NULL;
    int width = 0;
    int height = 0;
    size_t size = 0;
    int roiPercentage = m_roiPercetange;

    vector<int> newIndex;
    if (imgs.size() != mvs.size()) {
        mfllLogE("%s: the amount of image(%d) and GMV(%d) is different",
                __FUNCTION__, imgs.size(), mvs.size());
        goto lbExit;
    }

    width = imgs[0]->getWidth();
    height = imgs[0]->getHeight();
    size = imgs.size();

    mtkBss = MTKBss::createInstance(DRV_BSS_OBJ_SW);
    if (mtkBss == NULL) {
        mfllLogE("%s: create MTKBss instance failed", __FUNCTION__);
        goto lbExit;
    }

    if (mtkBss->BssInit(NULL, NULL) != S_BSS_OK) {
        mfllLogE("%s: init MTKBss failed", __FUNCTION__);
        goto lbExit;
    }

    /* set feature control */
    {
        /* calcuate ROI cropping width */
        int w = (width * roiPercentage + 5) / 100;
        int h = (height * roiPercentage + 5) / 100;
        int x = (width - w) / 2;
        int y = (height - h) / 2;

        mfllLogD("%s: BSS ROI(x,y,w,h)=(%d,%d,%d,%d)", __FUNCTION__, x, y, w, h);

        BSS_PARAM_STRUCT p;
        p.BSS_ON = 1;
        p.BSS_ROI_WIDTH = w;
        p.BSS_ROI_HEIGHT = h;
        p.BSS_ROI_X0 = x;
        p.BSS_ROI_Y0 = y;
        p.BSS_SCALE_FACTOR = CUST_MFLL_BSS_SCALE_FACTOR;
        p.BSS_CLIP_TH0 = CUST_MFLL_BSS_CLIP_TH0;
        p.BSS_CLIP_TH1 = CUST_MFLL_BSS_CLIP_TH1;
        p.BSS_ZERO = CUST_MFLL_BSS_ZERO;
        p.BSS_FRAME_NUM = (MUINT32)imgs.size();

        auto b = mtkBss->BssFeatureCtrl(BSS_FTCTRL_SET_PROC_INFO, (void*)&p, NULL);
        if (b != S_BSS_OK) {
            mfllLogE("%s: set info to MTKBss failed (%d)", __FUNCTION__, (int)b);
            goto lbExit;
        }
    }

    /* main process */
    {
        BSS_OUTPUT_DATA outParam;
        BSS_INPUT_DATA inParam;

        inParam.inWidth = width;
        inParam.inHeight = height;
        for (size_t i = 0; i < size; i++) {
            inParam.apbyBssInImg[i] = (MUINT8*)imgs[i]->getVa();
            inParam.gmv[i].x = mvs[i].x;
            inParam.gmv[i].y = mvs[i].y;
        }

        auto b = mtkBss->BssMain(BSS_PROC1, &inParam, &outParam);
        if (b != S_BSS_OK) {
            mfllLogE("%s: MTKBss::Main returns failed (%d)", __FUNCTION__, (int)b);
            goto lbExit;
        }

        for (size_t i = 0; i < size; i++) {
            newIndex.push_back(outParam.originalOrder[i]);
        }
    }

lbExit:
    if (mtkBss)
        mtkBss->destroyInstance();
    return newIndex;
}
