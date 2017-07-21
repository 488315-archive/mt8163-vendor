#define LOG_TAG "MtkCam/CamClient/OTClient"
//
#include "../inc/CamUtils.h"
using namespace android;
using namespace MtkCamUtils;
//
#include <stdlib.h>
#if (MTKCAM_BASIC_PACKAGE != 1)
#include <linux/cache.h>
#endif
//
#include "OTBufMgr.h"
//
#include <cutils/atomic.h>
//
/******************************************************************************
*
*******************************************************************************/
#include <Log.h>
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, arg...)    if (cond) { MY_LOGV(arg); }
#define MY_LOGD_IF(cond, arg...)    if (cond) { MY_LOGD(arg); }
#define MY_LOGI_IF(cond, arg...)    if (cond) { MY_LOGI(arg); }
#define MY_LOGW_IF(cond, arg...)    if (cond) { MY_LOGW(arg); }
#define MY_LOGE_IF(cond, arg...)    if (cond) { MY_LOGE(arg); }

#define OTCLIENT_ASSERT(x)          {if(!(x)) {CAM_LOGE("ASSERT FAILED:%s", #x);}}

#define FUNCTION_IN                 MY_LOGD("+")
#define FUNCTION_OUT                MY_LOGD("-")

/******************************************************************************
*
*******************************************************************************/
void
OTBuffer::
createBuffer()
{
    FUNCTION_IN;
    //
    mbufSize = 640*900*2; //Video WallPaper: Height > Width
    mbufSize = (mbufSize + L1_CACHE_BYTES-1) & ~(L1_CACHE_BYTES-1);
    mSize = mbufSize;

    mAllocator = IImageBufferAllocator::getInstance();
    IImageBufferAllocator::ImgParam imgParam(mSize,0);
    mpImg = mAllocator->alloc("OTBuffer", imgParam);
    OTCLIENT_ASSERT(mpImg != 0);
    OTCLIENT_ASSERT(mpImg->lockBuf("OTBuffer", (eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_MASK)));

    mpImg->syncCache(eCACHECTRL_INVALID);
    mVirtAddr = mpImg->getBufVA(0);
    mPhyAddr  = mpImg->getBufPA(0);
    mMemID    = mpImg->getFD(0);

    OTCLIENT_ASSERT(mVirtAddr);
    OTCLIENT_ASSERT(mPhyAddr);

    OTCLIENT_ASSERT((mSize & (L1_CACHE_BYTES-1)) == 0);
    OTCLIENT_ASSERT((mVirtAddr & (L1_CACHE_BYTES-1)) == 0);
    MY_LOGD("OTClient: Virtual Addr: 0x%x, Phyical Addr: 0x%x, bufSize:%d", mVirtAddr, mPhyAddr, mSize);

    FUNCTION_OUT;
}


/******************************************************************************
*
*******************************************************************************/
void
OTBuffer::
destroyBuffer()
{
    FUNCTION_IN;

    if (mAllocator) {
        if (mpImg) {
            mpImg->unlockBuf("OTBuffer");
            mAllocator->free(mpImg);
            mpImg = NULL;
        }
        mAllocator = NULL;
    }

    FUNCTION_OUT;
}

